/*
*
* Copyright(C) 2022 MiracleForest Studio. All Rights Reserved.
*
* @filename:error.hpp
* @creation time:2022.5.11.12:45
* @created by:Lovelylavender4
* -----------------------------------------------------------------------------
* 
* -----------------------------------------------------------------------------
* If you have contact or find bugs,
* you can go to Github or email (MiracleForest@Outlook.com) to give feedback.
* We will try to do our best!
*/
#ifndef ___MIRACLEFOREST_MEET_ERROR___
#define ___MIRACLEFOREST_MEET_ERROR___

#include <string>
#include <exception>

namespace meet {

	enum class Error : int {
		noError = 0,
		unkError = -1,
		initializationWinsockFailed = -16,
		socketError,
		bindError,
		listenError,
		acceptError,
		sendFailed,
		recvFailed,
		portTooSmall,
		maxcouTooBig,
		connectFailed,
		unsupportedOperations,
		connecting,
		noConnected,
		changeError
    };//enum class Error

	static std::string getString(Error errorCode) {
		switch (errorCode) {
			using enum Error;
		case noError: {
			return "noError";
		}
		case unkError: {
			return "unkError";
		}
		case initializationWinsockFailed: {
			return "initializationWinsockFailed";
		}
		case socketError: {
			return "socketError";
		}
		case bindError: {
			return "bindError";
		}
		case listenError: {
			return "listenError";
		}
		case acceptError: {
			return "acceptError";
		}
		case sendFailed: {
			return "sendFailed";
		}
		case recvFailed: {
			return "recvFailed";
		}
		case portTooSmall: {
			return "portTooSmall";
		}
		case maxcouTooBig: {
			return "maxcouTooBig";
		}
		case connectFailed: {
			return "connectFailed";
		}
		case unsupportedOperations: {
			return "unsupportedOperations";
		}
		case connecting: {
			return "connecting";
		}
		case noConnected: {
			return "noConnected";
		}
		case changeError: {
			return "changeError";
		}
		default: {
			return "Error! unk errorCode!";
		}
		}
	}//static std::string getString(Error errorCode)

}//namespace meet

#endif //!___MIRACLEFOREST_MEET_ERROR___