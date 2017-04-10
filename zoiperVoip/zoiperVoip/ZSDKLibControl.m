//==============================================================================
//  ZSDKLibControl.m
//  zoiper-sdk-demo
//
//  Created by Ivan Vasilev on 29/1/14.
//  Copyright (c) 2014 Securax Ltd. All rights reserved.
//==============================================================================



#import "ZSDKLibControl.h"
WrapperContext gWrapperCtx;
WrapperCallbacks * gWrapperCbk;
BOOL gInitialized = NO;
BOOL gLogStarted = NO;
BOOL gbRegistrationOk = NO;
BOOL gbInCall = NO;
BOOL gbActivated = NO;
int  gUserId = 0;
CallHandler gCallId;

// Video call settings
void * gVideoThreadId = NULL;

//==============================================================================
//  Callback declarations
//==============================================================================
void onStunNetworkDiscovered( StunHandler StunId, eNetworkTypeEnum_t netType );
void onStunPortReady( UserHandler user_handler, CallHandler call_handler,
                     void* user_data, LIBRESULT result );
static void onUserRegistered( UserHandler userId, const char * pAor, int newMsg, int oldMsg );
void onUserRegistrationFailure( UserHandler userId, int isRegister, int causeCode );
void onUserRegistrationRetrying( UserHandler UserId, int IsRegistering, int RetrySeconds );
void onUserUnregistered( UserHandler userId );
void onCallCreate( UserHandler UserID, CallHandler CallID, const char * pCallee ); // outgoing call
void onCallCreated( UserHandler UserID, CallHandler CallID, const char * pPeer,    // incoming call
                   const char * pPeerNumber, const char * pPeerURI,
                   const char * pDNID, int AutoAnswerSecs );
void onUnknownCall( CallHandler CallID, const char * pPeer, const char * pPeerNumber,
                   const char * pPeerURI, const char * pDNID );
void onCallAccept( CallHandler CallID, AudioCodecEnum_t codec,
                  eCallDirection_t call_direction );
void onCallHangup( CallHandler CallID, int CauseCode );
void onCallRinging( CallHandler CallID );
void onEarlyMedia( CallHandler CallID, AudioCodecEnum_t codec );
void onCallReject( CallHandler CallID, int CauseCode );
void onCallFailure( CallHandler CallID, int CauseCode );
void onCallDTMFResult( CallHandler CallID, LIBRESULT lRes );
void onGeneralFailure( ErrorSources_t errsrc, const char * msg, int causeCode );
static void onActivationCompleted( eActivationStatus_t status, const char * reason,
                           const char * certificate, const char * build,
                           const char * hddSerial, const char * mac,
                           const char * checksum );
void onVideoStarted(CallHandler CallId, void * pThreadId, AudioCodecEnum_t codec );
void onVideoStopped( CallHandler CallId, void * pThreadId);
void onVideoFormatSelected( CallHandler CallId, eCallDirection_t dir,
                                int width, int height, float fps );
void onVideoOffered( CallHandler CallId );


void InitLibrary(int SIPPort, int IAXPort)
{
	memset( &gWrapperCtx, 0, sizeof( gWrapperCtx ) );
	gWrapperCtx.ctxVersion = WRAPPER_CONTEXT_VERSION;
	if( LoadWrapperContext( &gWrapperCtx ) != L_OK )
	{
		return;
	}
    
	gInitialized = TRUE;
	//gWrapperCtx.StartResipLog( "/tmp/zoiper_logfile.txt" );
	LIBRESULT res;
	gWrapperCbk = 0;
	res = gWrapperCtx.InitCallbackTable( WRAPPER_CALLBACK_VERSION, &gWrapperCbk );
	if( res != L_OK )
	{
        NSLog(@"ERROR SETUP InitCallbackTable");
		gInitialized = NO;
		return;
	}
    
    // Handle user management callbacks
	gWrapperCbk->onUserRegistered           = onUserRegistered;
	gWrapperCbk->onUserRegistrationFailure  = onUserRegistrationFailure;
	gWrapperCbk->onUserRegistrationRetrying = onUserRegistrationRetrying;
	gWrapperCbk->onUserUnregistered         = onUserUnregistered;
	gWrapperCbk->onUserRegistrationFailure  = onUserRegistrationFailure;
    
    // Handle call management callbacks
	gWrapperCbk->onCallCreate               = onCallCreate;
	gWrapperCbk->onCallCreated              = onCallCreated;
	gWrapperCbk->onCallAccepted             = onCallAccept;
	gWrapperCbk->onCallHangup               = onCallHangup;
	gWrapperCbk->onCallRinging              = onCallRinging;
	gWrapperCbk->onCallEarlyMedia           = onEarlyMedia;
	gWrapperCbk->onCallRejected             = onCallReject;
	gWrapperCbk->onCallFailure              = onCallFailure;
	gWrapperCbk->onUnknownCall              = onUnknownCall;
    
    // Handle DTMF callbacks
	gWrapperCbk->onCallDTMFResult           = onCallDTMFResult;
    
    // Handle failure callback
	gWrapperCbk->onGeneralFailure           = onGeneralFailure;
    
    // Handle video management callbacks
    gWrapperCbk->onVideoStarted             = onVideoStarted;
    gWrapperCbk->onVideoStopped             = onVideoStopped;
    gWrapperCbk->onVideoOffered             = onVideoOffered;
    gWrapperCbk->onVideoFormatSelected      = onVideoFormatSelected;

    // Handle Activation status callback
    gWrapperCbk->onActivationCompleted      = onActivationCompleted;
    

    //
    gWrapperCtx.SetAudioDriverConfiguration(E_AUDIO_DRV_DEFAULT, 8000, 0);
    //gWrapperCtx.SetAudioResamplerType(E_AUDIO_DRV_RESAMPLER_IPHONE);
    
	res = gWrapperCtx.InitCallManager( gWrapperCbk, SIPPort, IAXPort );
	if( res != L_OK )
	{
        NSLog(@"ERROR SETUP InitCallManager");
		gInitialized = NO;
		return;
	}

	gInitialized = YES;
    
    NSLog(@"Finish SETUP");
}

void PollLibrary()
{
    if (gInitialized)
        gWrapperCtx.PollEvents();
}

//==============================================================================
// User management callbacks
//==============================================================================
void onUserRegistered( UserHandler userId, const char * pAor, int newMsg,
                        int oldMsg )
{
    gbRegistrationOk = YES;
    NSLog(@"ZOIPER: onUserRegistered");
    [[NSNotificationCenter defaultCenter]
            postNotificationName:@"ZSDKctxDidRegistrationSucceeded" object:nil];
}

void onUserRegistrationFailure(UserHandler userId, int isRegister, int causeCode)
{
    gbRegistrationOk = NO;
    NSLog(@"ZOIPER: onUserRegistrationFailure");
}

void onUserRegistrationRetrying( UserHandler UserId, int IsRegistering,
                                int RetrySeconds )
{
    NSLog(@"ZOIPER: onUserRegistrationRetrying");
}

void onUserUnregistered( UserHandler userId )
{
    gbRegistrationOk = NO;
    NSLog(@"ZOIPER: onUserUnregistered");
    [[NSNotificationCenter defaultCenter]
            postNotificationName:@"ZSDKctxDidRegistrationSucceeded" object:nil];
}

//==============================================================================
// Call management callbacks
//==============================================================================
void onCallCreate( UserHandler UserID, CallHandler CallID, const char * pCallee )
{
    gbInCall = YES;
    NSLog(@"ZOIPER: onCallCreate");
    [[NSNotificationCenter defaultCenter]
            postNotificationName:@"ZSDKctxDidCallStatusChanged" object:nil];
}

void onCallCreated( UserHandler UserID, CallHandler CallID, const char * pPeer,
                   const char * pPeerNumber, const char * pPeerURI,
                   const char * pDNID, int AutoAnswerSecs )
{
     NSLog(@"ZOIPER: onCallCreated");
}

void onUnknownCall( CallHandler CallID, const char * pPeer,
                   const char * pPeerNumber, const char * pPeerURI,
                   const char * pDNID )
{
}

void onCallAccept( CallHandler CallID, AudioCodecEnum_t codec,
                  eCallDirection_t call_direction )
{
}

void onCallHangup( CallHandler CallID, int CauseCode )
{
    gbInCall = NO;
    [[NSNotificationCenter defaultCenter]
            postNotificationName:@"ZSDKctxDidCallStatusChanged" object:nil];
}

void onCallRinging( CallHandler CallID )
{
}

void onEarlyMedia( CallHandler CallID, AudioCodecEnum_t codec )
{
}

void onCallReject( CallHandler CallID, int CauseCode )
{
    gbInCall = NO;
    NSLog(@"ZOIPER: onCallReject");
    [[NSNotificationCenter defaultCenter]
            postNotificationName:@"ZSDKctxDidCallStatusChanged" object:nil];
}

void onCallFailure( CallHandler CallID, int CauseCode )
{
    gbInCall = NO;
    NSLog(@"ZOIPER: onCallFailure");
    [[NSNotificationCenter defaultCenter]
            postNotificationName:@"ZSDKctxDidCallStatusChanged" object:nil];
}

//==============================================================================
// DTMF reception callback
//==============================================================================
void onCallDTMFResult( CallHandler CallID, LIBRESULT lRes )
{
}

//==============================================================================
// Video management callbacks
//==============================================================================

// Video negotiations have finished and the remove party is ready to receive
// video frames
void onVideoStarted( CallHandler CallId, void * pThreadId, AudioCodecEnum_t codec)
{
    if (gCallId == CallId)
    {
        gVideoThreadId = pThreadId;
        [[NSNotificationCenter defaultCenter]
            postNotificationName:@"ZSDKctxDidVideoStarted" object:nil];
    }
}

void onVideoStopped( CallHandler CallId, void * pThreadId )
{
    if ((gCallId == CallId) && (gVideoThreadId == pThreadId))
    {
        [[NSNotificationCenter defaultCenter]
            postNotificationName:@"ZSDKctxDidVideoStopped" object:nil];
    }
}

void onVideoFormatSelected( CallHandler CallId, eCallDirection_t dir,
                            int width, int height, float fps )
{
    // Nothing to do...
}

void onVideoOffered( CallHandler CallId )
{
    if (gCallId == CallId)
    {
        [[NSNotificationCenter defaultCenter]
         postNotificationName:@"ZSDKctxDidVideoOffered" object:nil];
    }
}

//==============================================================================
// Activation result callback
//==============================================================================
void onActivationCompleted( eActivationStatus_t status, const char * reason,
                            const char * certificate, const char * build,
                            const char * hddSerial, const char * mac,
                            const char * checksum )
{
    if (E_ACT_SUCCESS == status)
    {
        gbActivated = YES;
    }
    NSLog(@"ZOIPER: onActivationCompleted");
    [[NSNotificationCenter defaultCenter]
        postNotificationName:@"ZSDKctxDidActivationStatusUpdated" object:nil];
}

//==============================================================================
// General failure callback
//==============================================================================
void onGeneralFailure( ErrorSources_t errsrc, const char * msg, int causeCode )
{
}



