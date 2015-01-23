/*
 * Copyright (C) 2014 Beat Küng <beat-kueng@gmx.net>
 *  Created by Maarten Billemont on 2014-12-20.
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



#include "crypto.h"
#include "crypto_functions.h"

#if defined(__linux__) || defined(__APPLE__)
#include <arpa/inet.h>
#elif defined(_WIN32)
#include <winsock2.h>
#else
#error "unsupported platform: add include for htonl"
#endif


using namespace std;

MasterPassword::MasterPassword() {
}

MasterPassword::~MasterPassword() {
}

void MasterPassword::login(const std::string& user_name,
		const std::string& password) {
	logout();

	//calculate master key
	string key_scope = getScope(MPSiteVariantPassword);

	// Calculate the master key salt
	// master_key_salt = key_scope . #user_name . user_name
	string master_key_salt = key_scope;
	addIntToString(master_key_salt, user_name.length());
	master_key_salt += user_name;

	int ret = scrypt((const uint8_t*) password.c_str(), password.length(),
			(const uint8_t*) master_key_salt.c_str(), master_key_salt.length(),
			MP_N, MP_r, MP_p, m_master_key, MP_dkLen);

	if (ret != 0) throw CryptoException(CryptoException::Type_scrypt_failed);

	m_is_logged_in = true;
}

void MasterPassword::logout() {
	for (int i = 0; i < MP_dkLen; ++i)
		m_master_key[i] = 0;
	m_is_logged_in = false;
}

std::string MasterPassword::sitePassword(const Site& site) {

	if (!m_is_logged_in)
		throw CryptoException(CryptoException::Type_not_logged_in);

	string site_scope = getScope(site.m_variant);
	string site_password;

	// Calculate the site seed.
	// sitePasswordSeed = hmac-sha256( masterKey, siteScope . #siteName
	//   . siteName . siteCounter [. #siteContext . siteContext] )
	string site_password_seed_info;
	site_password_seed_info += site_scope;
	addIntToString(site_password_seed_info, site.m_name.length());
	site_password_seed_info += site.m_name;
	addIntToString(site_password_seed_info, site.m_counter);
	if (site.m_context.length() > 0) {
		addIntToString(site_password_seed_info, site.m_context.length());
		site_password_seed_info += site.m_context;
	}

	const int seed_len = 32; //because we use SHA256
	unsigned char site_password_seed[seed_len];
	const unsigned char* ret = HMAC_SHA256(m_master_key, MP_dkLen,
			(const uint8_t*) site_password_seed_info.c_str(),
			site_password_seed_info.length(), site_password_seed);
	if (!ret)
		throw CryptoException(CryptoException::Type_HMAC_SHA256_failed);


	// Determine the template.
	string templates = templateForType(site.m_type, site_password_seed[0]);
	if (templates.length() > seed_len-1) {
		THROW_s(EINVALID_PARAMETER, "template too long for password seed (%i)",
				templates.length());
	}

	// Encode the password from the seed using the template.
	site_password.resize(templates.length(), ' ');
	for (size_t c = 0; c < templates.length(); ++c) {
		site_password[c] = characterFromClass(templates[c], site_password_seed[c + 1]);
	}

	return site_password;
}

std::string MasterPassword::getScope(MPSiteVariant variant) {
	switch (variant) {
	case MPSiteVariantPassword:
		return "com.lyndir.masterpassword";
	case MPSiteVariantLogin:
		return "com.lyndir.masterpassword.login";
	case MPSiteVariantAnswer:
		return "com.lyndir.masterpassword.answer";
	default:
		THROW(EINVALID_PARAMETER);
	}
}

void MasterPassword::addIntToString(std::string& str, uint32_t val) {
	val = htonl(val);
	str.append((const char*)&val, sizeof(val)/sizeof(char));
}

std::string MasterPassword::templateForType(MPSiteType type,
		uint8_t seed_byte) {

	if (!(type & MPSiteTypeClassGenerated)) {
		THROW(EINVALID_PARAMETER);
	}

	switch (type) {
	case MPSiteTypeGeneratedMaximum: {
		const char *templates[] = { "anoxxxxxxxxxxxxxxxxx", "axxxxxxxxxxxxxxxxxno" };
		return templates[seed_byte % 2];
	}
	case MPSiteTypeGeneratedLong: {
		const char *templates[] = { "CvcvnoCvcvCvcv", "CvcvCvcvnoCvcv",
				"CvcvCvcvCvcvno", "CvccnoCvcvCvcv", "CvccCvcvnoCvcv",
				"CvccCvcvCvcvno", "CvcvnoCvccCvcv", "CvcvCvccnoCvcv",
				"CvcvCvccCvcvno", "CvcvnoCvcvCvcc", "CvcvCvcvnoCvcc",
				"CvcvCvcvCvccno", "CvccnoCvccCvcv", "CvccCvccnoCvcv",
				"CvccCvccCvcvno", "CvcvnoCvccCvcc", "CvcvCvccnoCvcc",
				"CvcvCvccCvccno", "CvccnoCvcvCvcc", "CvccCvcvnoCvcc",
				"CvccCvcvCvccno" };
		return templates[seed_byte % 21];
	}
	case MPSiteTypeGeneratedMedium: {
		const char *templates[] = { "CvcnoCvc", "CvcCvcno" };
		return templates[seed_byte % 2];
	}
	case MPSiteTypeGeneratedBasic: {
		const char *templates[] = { "aaanaaan", "aannaaan", "aaannaaa" };
		return templates[seed_byte % 3];
	}
	case MPSiteTypeGeneratedShort:
		return "Cvcn";
	case MPSiteTypeGeneratedPIN:
		return "nnnn";
	case MPSiteTypeGeneratedName:
		return "cvccvcvcv";
	case MPSiteTypeGeneratedPhrase: {
		const char *templates[] = { "cvcc cvc cvccvcv cvc",
				"cvc cvccvcvcv cvcv", "cv cvccv cvc cvcvccv" };
		return templates[seed_byte % 3];
	}
	default:
		THROW(EINVALID_PARAMETER);
	}
}

char MasterPassword::characterFromClass(char character_class,
		uint8_t seed_byte) {

	string classCharacters;
	switch (character_class) {
	case 'V':
		classCharacters = "AEIOU";
		break;
	case 'C':
		classCharacters = "BCDFGHJKLMNPQRSTVWXYZ";
		break;
	case 'v':
		classCharacters = "aeiou";
		break;
	case 'c':
		classCharacters = "bcdfghjklmnpqrstvwxyz";
		break;
	case 'A':
		classCharacters = "AEIOUBCDFGHJKLMNPQRSTVWXYZ";
		break;
	case 'a':
		classCharacters = "AEIOUaeiouBCDFGHJKLMNPQRSTVWXYZbcdfghjklmnpqrstvwxyz";
		break;
	case 'n':
		classCharacters = "0123456789";
		break;
	case 'o':
		classCharacters = "@&%?,=[]_:-+*$#!'^~;()/.";
		break;
	case 'x':
		classCharacters = "AEIOUaeiouBCDFGHJKLMNPQRSTVWXYZbcdfghjklmnpqrstvwxyz0123456789!@#$%^&*()";
		break;
	case ' ':
		classCharacters = " ";
		break;
	default:
		THROW(EINVALID_PARAMETER);
	}

    return classCharacters[seed_byte % classCharacters.length()];
}
