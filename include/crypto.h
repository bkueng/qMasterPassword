/*
 * Copyright (C) 2014 Beat Küng <beat-kueng@gmx.net>
 *  Created by Maarten Billemont on 2012-02-01.
 *  Copyright (c) 2014 Lyndir. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _HEADER_CRYPTO_H_
#define _HEADER_CRYPTO_H_


//scrypt parameters
#define MP_N                32768
#define MP_r                8
#define MP_p                2
//master key length
#define MP_dkLen            64


#include <string>
#include <stdint.h>
#include "exception.h"


typedef enum {
    /** Generate the password to log in with. */
            MPSiteVariantPassword,
    /** Generate the login name to log in as. */
            MPSiteVariantLogin,
    /** Generate the answer to a security question. */
            MPSiteVariantAnswer,
} MPSiteVariant;

typedef enum {
    /** Generate the password. */
            MPSiteTypeClassGenerated = 1 << 4,
    /** Store the password. */
            MPSiteTypeClassStored = 1 << 5,
} MPSiteTypeClass;

typedef enum {
    /** Export the key-protected content data. */
            MPSiteFeatureExportContent = 1 << 10,
    /** Never export content. */
            MPSiteFeatureDevicePrivate = 1 << 11,
} MPSiteFeature;

typedef enum {
    MPSiteTypeGeneratedMaximum = 0x0 | MPSiteTypeClassGenerated | 0x0,
    MPSiteTypeGeneratedLong = 0x1 | MPSiteTypeClassGenerated | 0x0,
    MPSiteTypeGeneratedMedium = 0x2 | MPSiteTypeClassGenerated | 0x0,
    MPSiteTypeGeneratedBasic = 0x4 | MPSiteTypeClassGenerated | 0x0,
    MPSiteTypeGeneratedShort = 0x3 | MPSiteTypeClassGenerated | 0x0,
    MPSiteTypeGeneratedPIN = 0x5 | MPSiteTypeClassGenerated | 0x0,
    MPSiteTypeGeneratedName = 0xE | MPSiteTypeClassGenerated | 0x0,
    MPSiteTypeGeneratedPhrase = 0xF | MPSiteTypeClassGenerated | 0x0,

    MPSiteTypeStoredPersonal = 0x0 | MPSiteTypeClassStored | MPSiteFeatureExportContent,
    MPSiteTypeStoredDevicePrivate = 0x1 | MPSiteTypeClassStored | MPSiteFeatureDevicePrivate,
} MPSiteType;

static inline int MPSiteTypeCount() { return 8; }
static inline MPSiteType MPSiteTypeFromIdx(int idx) {
	switch(idx) {
	case 0: return MPSiteTypeGeneratedMaximum;
	case 1: return MPSiteTypeGeneratedLong;
	case 2: return MPSiteTypeGeneratedMedium;
	case 3: return MPSiteTypeGeneratedBasic;
	case 4: return MPSiteTypeGeneratedShort;
	case 5: return MPSiteTypeGeneratedPIN;
	case 6: return MPSiteTypeGeneratedName;
	case 7: return MPSiteTypeGeneratedPhrase;
	default: THROW(EINVALID_PARAMETER);
	}
}
static inline int MPSiteTypeToIdx(MPSiteType type) {
	switch(type) {
	case MPSiteTypeGeneratedMaximum: return 0;
	case MPSiteTypeGeneratedLong: return 1;
	case MPSiteTypeGeneratedMedium: return 2;
	case MPSiteTypeGeneratedBasic: return 3;
	case MPSiteTypeGeneratedShort: return 4;
	case MPSiteTypeGeneratedPIN: return 5;
	case MPSiteTypeGeneratedName: return 6;
	case MPSiteTypeGeneratedPhrase: return 7;
	default: THROW(EINVALID_PARAMETER);
	}
}
static inline std::string MPSiteTypeToString(MPSiteType type) {
	switch(type) {
	case MPSiteTypeGeneratedMaximum: return "Maximum";
	case MPSiteTypeGeneratedLong: return "Long";
	case MPSiteTypeGeneratedMedium: return "Medium";
	case MPSiteTypeGeneratedBasic: return "Basic";
	case MPSiteTypeGeneratedShort: return "Short";
	case MPSiteTypeGeneratedPIN: return "PIN";
	case MPSiteTypeGeneratedName: return "Name";
	case MPSiteTypeGeneratedPhrase: return "Phrase";
	default: THROW(EINVALID_PARAMETER);
	}
}

/**
 ** class CryptoException
 */
class CryptoException {
public:
	enum CryptoExceptionType {
		Type_scrypt_failed = 0,
		Type_HMAC_SHA256_failed,
		Type_not_logged_in,
		Type_random_failed,
		Type_thread_exception,
	};
	CryptoException(CryptoExceptionType exception_type)
		: type(exception_type) {}

	const CryptoExceptionType type;
private:
};


/**
 ** class Site
 *  data for a single login. used to generate the actual site password
 */
class Site {
public:

	const std::string& getContext() const { return m_context; }
	void setContext(const std::string& context) { m_context = context; }

	uint32_t getCounter() const { return m_counter; }
	void setCounter(uint32_t counter) { m_counter = counter; }

	const std::string& getName() const { return m_name; }
	void setName(const std::string& name) { m_name = name; }

	MPSiteType getType() const { return m_type; }
	void setType(MPSiteType type) { m_type = type; }
	void setType(const std::string& type_str);

	MPSiteVariant getVariant() const { return m_variant; }
	void setVariant(MPSiteVariant variant) { m_variant = variant; }
	void setVariant(const std::string& variant);

private:
	std::string m_name;
	uint32_t m_counter = 1;

	MPSiteVariant m_variant = MPSiteVariantPassword;
	MPSiteType m_type = MPSiteTypeGeneratedLong;

	/* optional things */
	std::string m_context; /** type-specific context */

	friend class MasterPassword;
};

/**
 ** class User
 */
class User {
public:
	User(const std::string& user_name="") : m_user_name(user_name) {}

	const std::string& getPasswordHash() const { return m_password_hash; }
	const std::string& getSalt() const { return m_salt; }

	bool storePasswordHash() const { return m_store_password_hash; }
	void disableStorePasswordHash() { m_store_password_hash = false; m_password_hash = ""; }
	void setStorePasswordHash(const std::string& password);
	void setStoredHashData(const std::string& hash, const std::string& salt);

	const std::string& getUserName() const { return m_user_name; }
	void setUserName(const std::string& userName) { m_user_name = userName; }

	static std::string hash(const string& password, const string& salt);
private:
	std::string m_user_name;

	bool m_store_password_hash = false; /// if hash is stored, pw is verified on login
	std::string m_password_hash;
	std::string m_salt;
};

/**
 ** class MasterPassword
 * main class to handle master password and generate the site passwords.
 */
class MasterPassword {
public:
	MasterPassword();
	~MasterPassword();

	/**
	 * @return true if successfully logged in, false if wrong password
	 */
	bool login(const User& user, const std::string& password);
	void logout();
	bool isLoggedIn() const { return m_is_logged_in; }

	std::string sitePassword(const Site& site);

private:
	static std::string getScope(MPSiteVariant variant);

	/**
	 * concat string with an integer (the raw bytes in network order)
	 */
	static void addIntToString(std::string& str, uint32_t val);

	static std::string templateForType(MPSiteType type, uint8_t seed_byte);
	static char characterFromClass(char character_class, uint8_t seed_byte);

	bool m_is_logged_in = false;
	uint8_t m_master_key[MP_dkLen];
};


#endif /* _HEADER_CRYPTO_H_ */
