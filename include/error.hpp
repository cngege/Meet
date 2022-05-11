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

    enum class Error : int{
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
		connectFailed
    };//enum class Error

}//namespace meet

#endif //!___MIRACLEFOREST_MEET_ERROR___