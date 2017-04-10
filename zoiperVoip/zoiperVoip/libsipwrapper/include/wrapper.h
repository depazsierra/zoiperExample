#if( !defined( _WRAPPER_EXTERNAL_H_INCLUDED_ ) )
#define _WRAPPER_EXTERNAL_H_INCLUDED_

#include "wrapper_defs.h"
#include "wrapper_cbk.h"

#if( defined( __cplusplus ) )
extern "C" {
#endif

/** \brief Current wrapper context version.
 *
 *  New versions of the structure will be binary compatible with older
 *  version.  New function pointers will be added at the end of the
 *  structure.
 */
#define WRAPPER_CONTEXT_VERSION 46

/** \brief Wrapper Context
 *
 *  Contains pointers to the library's functions.
 */
typedef struct {
	/** \brief Structure version
	 *
	 *  The structure version.  Set it to the requested version before
	 *  calling LoadWrapperContext()
	 *
	 *  \sa LoadWrapperContext()
	 */
	int ctxVersion;

	/** \addtogroup functions Library functions
	 *  @{
	 */


	/** \addtogroup info Library information functions
	 *  @{
	 */

	/** \brief Get the library version.
	 *
	 *  Get the revision of the library's source last commit.
	 *
	 *  \return The revision as an ASCIIZ string.  The string resides
	 *          in the library's memory and should not be modified.
	 *
	 *  \sa GetLibraryDate()
	 */
	const char * (* GetLibraryVersion )( void );

	/** \brief Get the library date.
	 *
	 *  Get the date of the library's source last commit.
	 *
	 *  \return The build date as an ASCIIZ string.  The string resides
	 *          in the library's memory and should not be modified.
	 *
	 *  \sa GetLibraryVersion()
	 */
	const char * (* GetLibraryDate )( void );

	/** @} */

	/** \addtogroup init Library initialization and finalization functions
	 *  @{
	 */

	/** \brief Creates and clears a callback table.
	 *
	 *  Creates a callback table for the requested version and clears it.
	 *  This function will allocate the needed structure and will return
	 *  a pointer to it in ppCbk.  All callbacks will be pre-set with
	 *  no-op functions that can be replaced by the API user.  Function
	 *  pointers can be changed even after InitCallManager() but never
	 *  during a PollEvents() call.
	 *
	 *  \param[in] cbkVersion The requested version of the table
	 *  \param[out] ppCbk Pointer to a pointer to the table
	 *
	 *  \sa DestroyCallbackTable() and InitCallManager()
	 */
	LIBRESULT (* InitCallbackTable )( int cbkVersion, WrapperCallbacks ** ppCbk );

	/** \brief Destroys a callback table.
	 *
	 *  Destroys a callback table and frees the memory. Call
	 *  DestroyCallManager() before destroying a table that was given
	 *  to InitCallManager().
	 *
	 *  \param[in] pCbk Pointer to the callback table to destroy.
	 *
	 *  \sa InitCallbackTable(), InitCallManager() and DestroyCallManager()
	 */
	LIBRESULT (* DestroyCallbackTable )( WrapperCallbacks * pCbk );

	/** \brief Initialize the library.
	 *
	 *  This call will create all internal structures, protocol stacks,
	 *  network transports and event queues.  It will test the available
	 *  audio devices.  It will spawn necessary processing threads.
	 *  You must call DestroyCallManager() to close the network transports,
	 *  stop all threads and free all structures.
	 *
	 *  Do not call this function more than once in a row; use
	 *  DestroyCallManager() to clean up and then you can call
	 *  InitCallManager() again.
	 *
	 *  \param[in] pCbk A pointer to already configured callback table to use
	 *  \param[in] SIPPort Base SIP port number
	 *  \param[in] IAXPort Base IAX port, or 0 to disable the IAX call manager
	 *
	 *  \sa DestroyCallManager(), InitCallbackTable()
	 */
	LIBRESULT (* InitCallManager )( WrapperCallbacks * pCbk, WORD SIPPort, WORD IAXPort );

	/** \brief Destroys the library.
	 *
	 *  Closes network transports, terminates worker threads and frees
	 *  all structures.  You can call InitCallManager() after this to
	 *  re-open the library.
	 *
	 *  This is a blocking call and will always take some time to finish
	 *  but has a hard-limit of 2 seconds for IAX and 4 seconds for SIP
	 *  which in the worst case means 6-7 seconds with some additional time
	 *  for thread synchronisation.
	 *
	 *  \sa InitCallManager()
	 */
	LIBRESULT (* DestroyCallManager )( void );

	/** @} */

	/** \addtogroup global Global configuration functions
	 *  @{
	 */

	/** \brief Adds a codec to the default codec list.
	 *
	 *  Adds the codec to the list of codecs for users that do not have
	 *  their own lists created by using AddUserCodec().
	 *
	 *  The order in which codecs are added is important.  The codecs
	 *  that are added first have greater priority.  To reorder the
	 *  list clear it first and then re-add the codecs.
	 *
	 *  \param[in] eCodec The codec to be added
	 *
	 *  \sa ClearCodecList(), AddUserCodec() and ClearUserCodecList()
	 */
	LIBRESULT (* AddCodec )( CodecEnum_t eCodec );

	/** \brief Clears the default codec list.
	 *
	 *  Clears the default codec list managed by AddCodec().
	 *  \sa AddCodec(), AddUserCodec() and ClearUserCodecList()
	 */
	LIBRESULT (* ClearCodecList )( void );

	/** @} */

	/** \addtogroup user User management functions
	 *  @{
	 */

	/** \brief Adds a codec to the user's own codec list.
	 *
	 *  Adds the codec to the user's own codec list. If this function is
	 *  used the default codec list will no longer be used by this account.
	 *
	 *  The order in which codecs are added is important.  The codecs
	 *  that are added first have greater priority.  To reorder the
	 *  list clear it first and then re-add the codecs.
	 *
	 *  \param[in] UserId The user id of the account
	 *  \param[in] eCodec The codec to be added
	 */
	LIBRESULT (* AddUserCodec )( UserHandler UserId, CodecEnum_t eCodec );

	/** \brief Clears the user's codec list and forces the default list
	 *
	 *  Clears the user's own codec list. The user will start using the
	 *  default codec list until AddUserCodec() is called to re-create it.
	 *
	 *  \param[in] UserId the user id of the account
	 */
	LIBRESULT (* ClearUserCodecList )( UserHandler UserId );

	/** \brief Creates a new user account
	 *
	 *  Creates a new SIP or IAX2 user that can be used to register on
	 *  a server for incoming calls, create outgoing calls, subscribe for
	 *  presence, etc.  This is a mandatory operation before using most
	 *  of the library's functions.
	 *
	 *  This call will only prepare the structures for the user account.
	 *  It will not be registered to the server until RegisterUser() is
	 *  called (note that registration is not a mandatory operation).
	 *
	 *  \param[in] Proto The protocol (SIP and IAX2 supported)
	 *  \param[in] pszName IAX2: User name used for authentication.
	 *             SIP: User name used for identification and authentication
	 *             unless SetAuthUsername() is called.
	 *  \param[in] pszPassw User password to be used for authentication
	 *  \param[in] pszOutboundProxy IAX2: the server address; SIP: optional
	 *             SIP proxy server to force instead of the automatically
	 *             detected one.
	 *  \param[in] pszRealm IAX2: the context to be used; SIP: the user domain.
	 *             For sip accounts this is the domain part of the address of
	 *             record.  This is a mandatory parameter; it is used to
	 *             construct the account's AoR and also used by the SIP stack
	 *             to detect server settings (DNS SRV).
	 *  \param[in] pszCallerId SIP: used as the display name part in the
	 *             address of record
	 *  \param[in] pszCallerNumber IAX2: user name used for identification
	 *
	 *  \returns The handle of the newly created user account object or
	 *           INVALID_HANDLE on error
	 */
	UserHandler (* AddUser )( ProtoType_t Proto,
	                          const char * pszName, 
	                          const char * pszPassw, 
	                          const char * pszOutboundProxy, 
	                          const char * pszRealm, 
	                          const char * pszCallerId,
	                          const char * pszCallerNumber );

	/** \brief Destroys an user account object
	 *
	 *  Destroys an user account object and all related structures.
	 *  Fails if the user has active calls.
	 *
	 *  \param[in] userId The user id of the account to be destroyed
	 */
	LIBRESULT (* RemoveUser )( UserHandler userId );

	/** \brief Registers the user to the configured service
	 *
	 *  Starts the user's registration process.  If the registration process
	 *  was started the function returns L_OK.  The result of the process
	 *  will be delivered by a callback.
	 *
	 *  \param[in] userId The user id of the account to be registered
	 *
	 *  \sa WrapperCallbacks::onUserRegistered(),
	 *      WrapperCallbacks::onUserRegistrationFailure() and
	 *      WrapperCallbacks::onUserRegistrationRetrying()
	 */
	LIBRESULT (* RegisterUser )( UserHandler userId );

	/** \brief Cancels registration and/or unregisters the user
	 *
	 *  If the user account is in the process of registration this function
	 *  will cancel it.  If the user was already registered an unregistration
	 *  process will start.  If there is no error the function will return
	 *  immediately and the final result will be delivered via a callback.
	 *
	 *  \param[in] userId The user id of the account to be unregistered
	 *
	 *  \sa WrapperCallbacks::onUserUnregistered(),
	 *      WrapperCallbacks::onUserRegistrationFailure()
	 */
	LIBRESULT (* UnregisterUser )( UserHandler userId );

	/** @} */

	/** \deprecated Needed functionality was added to AddUser()
	 */
	LIBRESULT (* SetUserSipRegistrar )( UserHandler userId, const char * pszRegistrar );

	/** \deprecated Echo cancelling is a global option now controlled by UseEchoCancellation()
	 */
	LIBRESULT (* SetUserECState )( UserHandler userId, int state );

	/** \brief Selects the SIP transport for a SIP user account
	 *
	 *  \ingroup user
	 *
	 *  Only valid for SIP user accounts.  This function selects the transport
	 *  for the user.  The default is UDP.  This function must be called
	 *  right after AddUser() for best effect.
	 *
	 *  \param[in] userId The SIP user id to configure
	 *  \param[in] proto The signalling protocol to select (UDP, TCP, TLS)
	 */
	LIBRESULT (* SetUserTransport )( UserHandler userId, eUserTransport_t proto );

	/** \brief Changes the user name used for SIP authentication
	 *
	 *  \ingroup user
	 *
	 *  Only valid for SIP user accounts.  Changes the user name to be used
	 *  when responding to a SIP authentication challenge.  This function must
	 *  be called right after AddUser() for best effect.
	 *
	 *  The SIP user might be challenged on any SIP transaction (registration,
	 *  call creation, etc).  All authentication is handled by the library.
	 *
	 *  \param[in] userId The account user id to configure
	 *  \param[in] pszAuthUsername The new user name to use for authentication
	 */
	LIBRESULT (* SetAuthUsername )( UserHandler userId, const char * pszAuthUsername );

	/** \addtogroup call Call management functions
	 *  @{
	 */

	/** \brief Creates and starts an outgoing call
	 *
	 *  Creates a call originating from the specified user and starts it.
	 *  If there is no error the function will return immediately. Updates
	 *  on the call status will come via callbacks.
	 *
	 *  \param[in] userId The account's user id to use for originating the call.
     *             No registration is needed to make outgoing calls.
	 *  \param[in] pCallee The number/user to call. The actual address is created
	 *             from this id and the account's configued domain.
	 *  \param[out] CallId The newly created call handler
	 *
	 *  \sa WrapperCallbacks::onCallCreate(), WrapperCallbacks::onCallAccepted(),
	 *      WrapperCallbacks::onCallEarlyMedia(), WrapperCallbacks::onCallCodecNegotiated(),
	 *      WrapperCallbacks::onCallRinging(), WrapperCallbacks::onCallRejected(),
	 *      WrapperCallbacks::onCallHold(), WrapperCallbacks::onCallUnhold(),
	 *      WrapperCallbacks::onCallHangup(), WrapperCallbacks::onCallFailure()
	 */
	LIBRESULT (* CallCreate )( UserHandler userId, const char * pCallee, CallHandler * CallId );

	/** \brief Notifies the remote party that we are ringing (incoming calls)
	 *
	 *  Sends a ringing notification to the remote party for an incoming call.
	 *  The call must not be accepted yet.
	 *
	 *  \param[in] callId The incoming call id for which to send ringing notification
	 *
	 *  \sa WrapperCallbacks::onCallRinging()
	 */
	LIBRESULT (* CallRinging )( CallHandler callId );

	/** \brief Accepts an incoming call
	 *
	 *  Accepts an incoming call and opens audio and video channels for communication.
	 *  Due to legacy limitations there is no indication if an incoming call will have
	 *  video.  Always use CallEnableVideo() or CallDisableVideo() before accepting a
	 *  call to indicate if the call should accept video if offered.
	 *
	 *  \param[in] callId The incoming call to accept
	 *
	 *  \sa CallEnableVideo(), CallDisableVideo()
	 */
	LIBRESULT (* CallAccept )( CallHandler callId );

	/** \brief Rejects an incoming call
	 *
	 *  Rejects an incoming call with status busy.  The call must not be accepted yet.
	 *
	 *  \param[in] callId The incoming call to reject
	 *
	 *  \sa CallAccept(), CallHangup()
	 */
	LIBRESULT (* CallReject )( CallHandler callId );

	/** \brief Puts a call on hold
	 *
	 *  Puts a call on hold.  This function only initiates the process of putting the
	 *  call on hold.  If there is no error the audio is immediately muted and the
	 *  process of putting the call on hold is started.
	 *
	 *  In case the local Music on Hold service is enabled there will be no
	 *  protocol-level hold request sent to the other end. The audio stream
	 *  will be immediately switched to the music service.
	 *
	 *  In case a protocol-level hold cannot be sent (in some stages of a SIP call)
	 *  the call will be immediately muted and an actual hold request will be
	 *  deferred until either the call enters a suitable state or the API user
	 *  decides to CallUnhold() the call.
	 *
	 *  In all cases that cause this function to return "L_OK" there will be a
	 *  WrapperCallbacks::onCallHoldCompleted() event. In case the hold request
	 *  has been deferred, or there is Music Service, the event will be immediate.
	 *  Only in case the WRAPPER will send a protocol-level hold request the
	 *  onCallHoldCompleted() event will wait until the protocol-level request
	 *  has been completed.
	 *
	 *  It is safe to call CallUnhold() before onCallHoldCompleted() has arrived.
	 *  This will either enqueue the unhold request or it will cancel a deferred
	 *  hold request.
	 *
	 *  \param[in] callId The call to put on hold
	 *
	 *  \sa CallUnhold(), WrapperCallbacks::onCallHold(),
	 *      WrapperCallbacks:onCallHoldCompleted()
	 */
	LIBRESULT (* CallHold )( CallHandler callId );

	/** \brief Retrieves a call that was put on hold
	 *
	 *  Retrieves a call that has been previously put on hold. If there is no error
	 *  the function will start the process of restoring the call.
	 *
	 *  See CallHold() for more details about asynchronous execution.
	 *
	 *  In all cases where CallUnhold() returns "L_OK" there will be also
	 *  a WrapperCallbacks::onCallUnholdCompleted() event when the unhold
	 *  process has completed.
	 *
	 *  \param[in] callId The call to retrieve
	 *
	 *  \sa CallHold(), WrapperCallbacks::onCallUnhold(),
	 *      WrapperCallbacks::onCallUnholdCompleted()
	 */
	LIBRESULT (* CallUnhold )( CallHandler callId );

	/** \brief Initiates an unattended call transfer
	 *
	 *  Initiates an unattended transfer (also called blind transfer).  This function
	 *  can be used for incoming SIP calls to redirect them.  Incoming IAX2 calls
	 *  cannot be redirected.
	 *
	 *  Calls that were answered (incoming or outgoing) can be transferred using
	 *  this function (both SIP and IAX2).
	 *
	 *  The function returns immediately.  If there is no error the process of
	 *  unattended transfer will start.  The result of the transfer will be sent
	 *  via callbacks.
	 *
	 *  \param[in] callId The call to transfer
	 *  \param[in] pTransferee The destination to transfer the call to. Must be
	 *             registered on the same server.
	 *
	 *  \sa AttendedCallTransfer(), WrapperCallbacks::onCallTransferStarted(),
	 *      WrapperCallbacks::onCallTransferSucceeded(),
	 *      WrapperCallbacks::onCallTransferFailure()
	 */
	LIBRESULT (* UnattendedCallTransfer )( CallHandler callId, const char * pTransferee );

	/** \brief Initiates an attended call transfer
	 *
	 *  Initiates an attended call transfer.  There have to be two calls using
	 *  the same user account (call direction doesn't matter).  The calls
	 *  must have been accepted.  They can be currently on hold.
	 *
	 *  The function returns immediately.  If the transfer can be initiated
	 *  the function will return L_OK and the result of the transfer will be
	 *  delivered via callback.
	 *
	 *  The library will put the calls on hold if they already weren't. If
	 *  the transfer fails the first call will be retrieved back.
	 *
	 *  For SIP calls the REFER message will be sent to the first call.
	 *
	 *  \param[in] callId The first call to transfer
	 *  \param[in] toCallId The second where the first call should be transferred to
	 *
	 *  \sa UnattendedCallTransfer(), WrapperCallbacks::onCallTransferStarted(),
	 *      WrapperCallbacks::onCallTransferSucceeded(),
	 *      WrapperCallbacks::onCallTransferFailure()
	 */
	LIBRESULT (* AttendedCallTransfer )( CallHandler callId, CallHandler toCallId );

	/** \brief Hang up a call
	 *
	 *  Hangs up a call with normal status.
	 *
	 *  \param[in] callId Call to hang up
	 *
	 *  \sa CallReject()
	 */
	LIBRESULT (* CallHangup )( CallHandler callId );

	/** \brief Sends a DTMF signal over a call
	 *
	 *  Sends a DTMF signal over a call.  The BS (backspace) key is only
	 *  supported by SIP/KPML.  To select the DTMF type use SetUserDtmfBand().
	 *
	 *  The function returns immediately.  If there is no immediate error
	 *  the function will return L_OK and the actual status of the DTMF
	 *  send operation will be delivered via a callback.
	 *
	 *  The library will not produce an audible local signal to reflect
	 *  the DTMF transmission.  You can use AddSound() and StartSound()
	 *  to produce an audible signal for the user.  To send an actual
	 *  DTMF signal over the audio channel (inband) use StartPlayback().
	 *
	 *  \param[in] CallId Call to send the DTMF over
	 *  \param[in] DTMF The key to send
	 *
	 *  \sa SetUserDtmfBand(), GenerateDTMF(), GenerateSamples(),
	 *      AddSound(), StartSound(), StartPlayback(),
	 *      WrapperCallbacks::onCallDTMFResult()
	 */
	LIBRESULT (* CallSendDtmf )( CallHandler CallId, eDtmfCode_t DTMF );

	/** @} */

	/** \addtogroup record Call recording functions
	 *  @{
	 */

	/** \brief Opens a call recording file
	 *
	 *  Opens a call recording file.  The file will record only the
	 *  audio.  The file format will be windows WAV file containing the
	 *  uncompressed audio samples.
	 *
	 *  \param[in] CallId The call to record
	 *  \param[in] filename The file name to save the recording (limited to
	 *             ASCII characters on Windows or UTF-8 on linux and Mac).
	 *  \param[in] eType Selects the sides to record (local, remote, both).
	 *
	 *  \sa CallStartRecordInFile(), CallStopRecordInFile(), CallCloseFile()
	 */
	LIBRESULT (* CallOpenFile )( CallHandler CallId, const char * filename, eCallRecording_t eType );

	/** \brief Starts recording
	 *
	 *  Starts call recording in the associated file.  CallOpenFile() must be
	 *  called before this function.
	 *
	 *  \param[in] CallId The call to start recording
	 *
	 *  \sa CallOpenFile(), CallStopRecordInFile(), CallCloseFile()
	 */
	LIBRESULT (* CallStartRecordInFile )( CallHandler CallId );

	/** \brief Stops recording
	 *
	 *  Stops call recording.  The recording can be restarted again using
	 *  CallStartRecordInFile().  There is no need to explicitly stop a
	 *  recording.  It will be stopped automatically when the call is hung
	 *  up.
	 *
	 *  \param[in] CallId The call to stop recording
	 *
	 *  \sa CallOpenFile(), CallStartRecordInFile(), CallCloseFile()
	 */
	LIBRESULT (* CallStopRecordInFile )( CallHandler CallId );

	/** \brief Closes a recording file
	 *
	 *  Stops call recording (if started) and closes the recording file.
	 *  There is no need to explicitly close a recording file.  When a call
	 *  is hung up the recording will be stopped automatically and the file
	 *  will be closed.
	 *
	 *  \param[in] CallId The call for which to close the recording file
	 *
	 *  \sa CallStartRecordInFile(), CallStopRecordInFile(), CallOpenFile()
	 */
	LIBRESULT (* CallCloseFile )( CallHandler CallId );

	/** @} */

	/** \deprecated Echo cancellation is a global setting now
	 */
	LIBRESULT (* SetCallECState )( CallHandler CallId, int state );

	/** \addtogroup audio Audio management functions
	 *  @{
	 */

	/** \brief Mute or unmute the input device
	 *
	 *  Mutes or unmutes the input device.  The \p CallId parameter is
	 *  obsolete and unused and should be set to INVALID_HANDLE.
	 *
	 *  \param[in] CallId Unused. Pass INVALID_HANDLE here.
	 *  \param[in] bMute Set to 1 to mute and 0 to unmute the input device
	 *
	 *  \sa SVL_MuteOutput()
	 */
	LIBRESULT (* SVL_MuteInput )( CallHandler CallId, int bMute );

	/** \brief Mute or unmute the output device
	 *
	 *  Mutes or unmutes the output device.  The \p CallId parameter is
	 *  obsolete and unused and should be set to INVALID_HANDLE.
	 *
	 *  \param[in] CallId Unused. Pass INVALID_HANDLE here.
	 *  \param[in] bMute Set to 1 to mute and 0 to unmute the output device
	 *
	 *  \sa SVL_MuteInput()
	 */
	LIBRESULT (* SVL_MuteOutput )( CallHandler CallId, int bMute );

	/** @} */

	/** \brief Set the base port to use for RTP streams
	 *
	 *  \ingroup global
	 *
	 *  Set the base UDP port to use for RTP streams (SIP only).  Default is
	 *  8000.  This setting takes effect on the next SIP call.
	 *
	 *  Each SIP call uses two UDP ports, one for the actual audio stream
	 *  and another one for the control (RTCP) packets.  The first port is
	 *  always even.
	 *
	 *  The lib will search for a free even port from that base upwards for
	 *  each call.  When the call is finished the port will be reused in the
	 *  future.
	 *
	 *  \param[in] RTPPort The new RTP port base
	 *
	 *  \sa SetRTPSessionName(), SetRTPUsername(), SetRTPUrl(), SetRTPEmail()
	 */
	LIBRESULT (* SetRTPAudioPort )( WORD RTPPort );

	/** \brief Set the RTP session name for the SDP
	 *
	 *  \ingroup global
	 *
	 *  Sets the RTP session name for the SDP offers/answers.  The name should not
	 *  contain spaces.
	 *
	 *  \param[in] pName RTP session name
	 *
	 *  \sa SetRTPAudioPort(), SetRTPUsername(), SetRTPUrl(), SetRTPEmail()
	 */
	LIBRESULT (* SetRTPSessionName )( const char * pName );

	/** \deprecated The library automatically detects the address to use
	 */
	LIBRESULT (* SetRTPConnectionAddress )( DWORD IP );

	/** \deprecated The library automatically detects the address to use
	 */
	LIBRESULT (* SetRTPOriginAddress )( DWORD IP );

	/** \brief Set the RTP user name for the SDP
	 *
	 *  \ingroup global
	 *
	 *  Sets the RTP user name for the SDP offers/answers.  The user name
	 *  should not contain spaces.
	 *
	 *  \param[in] pName RTP user name
	 *
	 *  \sa SetRTPAudioPort(), SetRTPSessionName(), SetRTPUrl(), SetRTPEmail()
	 */
	LIBRESULT (* SetRTPUsername )( const char * pName );

	/** \brief Set the URL for SDP
	 *
	 *  \ingroup global
	 *
	 *  Sets the URL for the SDP offers/answers.  This is optional.
	 *
	 *  \param[in] pUrl the URL
	 *
	 *  \sa SetRTPAudioPort(), SetRTPSessionName(), SetRTPUsername(), SetRTPEmail()
	 */
	LIBRESULT (* SetRTPUrl )( const char * pUrl );

	/** \brief Set the e-mail address for SDP
	 *
	 *  \ingroup global
	 *
	 *  Sets the email address to put in SDP offers/answers.  This is optional.
	 *
	 *  \param[in] pEmail the e-mail address
	 *
	 *  \sa SetRTPAudioPort(), SetRTPSessionName(), SetRTPUsername(), SetRTPUrl()
	 */
	LIBRESULT (* SetRTPEmail )( const char * pEmail );

	/** \brief Select the input audio device
	 *
	 *  \ingroup audio
	 *
	 *  Select the input audio device.  The device id is a number in the range
	 *  0 .. (numAudioDevices-1).  Use GetNumberOfAudioDevices() to get the number
	 *  of audio devices; GetAudioDeviceInfo() and GetAudioDeviceType() can be
	 *  used to check if a device has input capabilities.
	 *
	 *  \param[in] DeviceId The device to select for input
	 *
	 *  \sa SetCurrentOutputDevice(), SetCurrentRingDevice(), GetCurrentInputDevice(),
	 *      GetNumberOfAudioDevices(), GetDefaultInputDevice(),
	 *      GetAudioDeviceInfo(), GetAudioDeviceType()
	 */
	LIBRESULT (* SetCurrentInputDevice )( int DeviceId );

	/** \brief Select the output audio device
	 *
	 *  \ingroup audio
	 *
	 *  Select the output audio device.  The device id is a number in the range
	 *  0 .. (numAudioDevices-1).  Use GetNumberOfAudioDevices() to get the number
	 *  of audio devices; GetAudioDeviceInfo() and GetAudioDeviceType() can be
	 *  used to check if a device has output capabilities.
	 *
	 *  \param[in] DeviceId The device to select for output
	 *
	 *  \sa SetCurrentInputDevice(), SetCurrentRingDevice(), GetCurrentOutputDevice(),
	 *      GetNumberOfAudioDevices(), GetDefaultOutputDevice(),
	 *      GetAudioDeviceInfo(), GetAudioDeviceType()
	 */
	LIBRESULT (* SetCurrentOutputDevice )( int DeviceId );

	/** \brief Select the output audio device to use for ringing
	 *
	 *  \ingroup audio
	 *
	 *  Select the output audio device to be used for ringing.  The device has to
	 *  be a vaild output audio device.  The device id is a number in the range
	 *  0 .. (numAudioDevices-1).  Use GetNumberOfAudioDevices() to get the number
	 *  of audio devices; GetAudioDeviceInfo() and GetAudioDeviceType() can be
	 *  used to check if a device has output capabilities.
	 *
	 *  \param[in] DeviceId The device to select for ringing
	 *
	 *  \sa SetCurrentOutputDevice(), SetCurrentInputDevice(), GetCurrentRingDevice(),
	 *      GetNumberOfAudioDevices(), GetDefaultOutputDevice(),
	 *      GetAudioDeviceInfo(), GetAudioDeviceType()
	 */
	LIBRESULT (* SetCurrentRingDevice )( int DeviceId );

	/** \brief Get the currently selected input device
	 *
	 *  \ingroup audio
	 *
	 *  Get the currently selected input device.
	 *
	 *  \return The currently selected input device or negative on error.
	 *
	 *  \sa GetCurrentOutputDevice(), GetCurrentRingDevice(),
	 *      SetCurrentInputDevice(), GetNumberOfAudioDevices(),
	 *      GetDefaultInputDevice()
	 */
	int (* GetCurrentInputDevice )( void );

	/** \brief Get the currently selected output device
	 *
	 *  \ingroup audio
	 *
	 *  Get the currently selected output device.
	 *
	 *  \return The currently selected output device or negative on error.
	 *
	 *  \sa GetCurrentInputDevice(), GetCurrentRingDevice(),
	 *      SetCurrentOutputDevice(), GetNumberOfAudioDevices(),
	 *      GetDefaultOutputDevice()
	 */
	int (* GetCurrentOutputDevice )( void );

	/** \brief Get the currently selected output device to use for ringing
	 *
	 *  \ingroup audio
	 *
	 *  Get the currently selected output device to be used for ringing.
	 *
	 *  \return The currently selected output device to use for ringing
	 *          or negative on error.
	 *
	 *  \sa GetCurrentOutputDevice(), GetCurrentInputDevice(),
	 *      SetCurrentRingDevice(), GetNumberOfAudioDevices(),
	 *      GetDefaultOutputDevice()
	 */
	int (* GetCurrentRingDevice )( void );

	/** \brief Get the total number of audio devices in the system
	 *
	 *  \ingroup audio
	 *
	 *  Get the total number of audio devices in the system.
	 *
	 *  \return The number of audio devices or negative on error
	 *
	 *  \sa GetAudioDeviceInfo(), GetAudioDeviceType()
	 */
	int (* GetNumberOfAudioDevices )( void );

	/** \brief Get the default input device in the system
	 *
	 *  \ingroup audio
	 *
	 *  Get the default input device in the system.  Note that this is not
	 *  the currently selected input device (see GetCurrentInputDevice()).
	 *
	 *  \return The default input device in the system
	 *
	 *  \sa GetCurrentInputDevice()
	 */
	int (* GetDefaultInputDevice )( void );

	/** \brief Get the default output device in the system
	 *
	 *  \ingroup audio
	 *
	 *  Get the default output device in the system.  Note that this is not
	 *  the currently selected output device (see GetCurrentOutputDevice()).
	 *
	 *  \return The default input device in the system
	 *
	 *  \sa GetCurrentOutputDevice(), GetCurrentRingDevice()
	 */
	int (* GetDefaultOutputDevice )( void );

	/** \brief Get information about an audio device
	 *
	 *  \ingroup audio
	 *
	 *  Get information about an audio device.  This function is mainly
	 *  provided to get the name of the audio device and to check wheter
	 *  it is an input or an output device (or both).  There is a helper
	 *  function for the latter, GetAudioDeviceType().
	 *
	 *  The returned structure resides in the library's memory and should
	 *  not be altered or freed.
	 *
	 *  \param[in] deviceId The device id to get information for
	 *
	 *  \return The device information
	 *
	 *  \sa GetNumberOfAudioDevices(), GetAudioDeviceType()
	 */
	const sAudioDeviceInfo_t * (* GetAudioDeviceInfo )( int deviceId );

	/** \brief Get the audio device type
	 *
	 *  \ingroup audio
	 *
	 *  Get the audio device type (input, output or both). The function
	 *  will count the number of input and output channels of the audio
	 *  device and return one of four possibilities:
	 *  \li \c eAudioInputDevice The device has only input
	 *  \li \c eAudioOutputDevice The device has only output
	 *  \li \c eAudioInOutDevice The device has both input and output
	 *  \li \c eUnknownAudioDevice An error has occured
	 *
	 *  \param[in] audioInfo The structure returned by GetAudioDeviceInfo()
	 *
	 *  \return The type of the audio device.
	 */
	eAudioDeviceType_t (* GetAudioDeviceType )( const sAudioDeviceInfo_t * audioInfo );

	/** \brief Generates an 8000Hz DTMF signal
	 *
	 *  \ingroup audio
	 *
	 *  Generates an 8000 Hz DTMF signal by using the GenerateSamples() function.
	 *
	 *  \param[in] DTMF The DTMF key to generate
	 *  \param[in] ToneLen Number of samples to generate
	 *  \param[out] pSampleBuffer Buffer for the generated samples. Must be at least
	 *              ( \p ToneLen * 2 ) bytes
	 *
	 *  \sa GenerateSamples()
	 */
	void (* GenerateDTMF )( eDtmfCode_t DTMF, WORD ToneLen, SAMPLE * pSampleBuffer);

	/** \brief Generates a signal with two frequencies
	 *
	 *  \ingroup audio
	 *
	 *  Generates an 8000Hz signal with two sinusoidal frequencies.
	 *
	 *  \param[in] freq1 The first frequence in Hz
	 *  \param[in] freq2 The second frequence in Hz
	 *  \param[out] pBuffer Output buffer, at least ( \p samples * 2 ) bytes
	 *  \param[in] samples Number of samples to generate
	 *
	 *  \sa GenerateDTMF()
	 */
	void (* GenerateSamples )( WORD freq1, WORD freq2, SAMPLE * pBuffer, WORD samples );

	/** \addtogroup event Callback management functions
	 *  @{
	 */

	/** \brief Polls the event queues
	 *
	 *  Use this function to check the event queues.  If there are events waiting
	 *  the registered callbacks will be invoked one at a time.
	 */
	void (* PollEvents )( void );

	/** @} */

	/** \addtogroup stun STUN management functions
	 *  @{
	 */

	/** \brief Creates a new STUN server configuration
	 *
	 *  The newly created STUN server configuration ID will be returned in the
	 *  \p StunId parameter.  The configuration will be empty and must be
	 *  filled in before the server can be started.
	 *
	 *  \param[out] StunId Pointer to a handler that will receive the new id
	 *
	 *  \sa RemoveStunServer(), AssignStunServer(), SetDefaultStunServer(),
	 *      SetStunServer(), SetStunPort(), SetStunRefreshPeriod(),
	 *      StartStunResolve(), StopStunResolve(), GetStunServer(),
	 *      GetStunPort(), GetStunResolvedAddress(), GetStunResolvedPort(),
	 *      GetStunNetworkType(), IsStunEnabled(), IsStunResolved()
	 *      PrepareStunPort()
	 */
	LIBRESULT (* AddStunServer )( StunHandler * StunId );

	/** \brief Destroys a STUN server
	 *
	 *  Destroys a STUN server.  There is a known issue with this function
	 *  that could cause the library to crash.  Do not use it.
	 *
	 *  \param[in] StunId The STUN server to destroy
	 */
	LIBRESULT (* RemoveStunServer )( StunHandler StunId );

	/** \brief Assigns a STUN server to a SIP user
	 *
	 *  Works only for SIP user accounts (IAX2 doesn't need STUN).  This will
	 *  assign the STUN server to a user account.  If the STUN configuration
	 *  is deemed usable it will be automatically used for incoming calls.
	 *  PrepareStunPort() must be used explicitly for outgoing calls.
	 *
	 *  \param[in] StunId The STUN server id to assign
	 *  \param[in] UserId The id of the user account
	 */
	LIBRESULT (* AssignStunServer )( StunHandler StunId, UserHandler UserId );

	/** \brief Assigns a STUN server for unknown incoming calls
	 *
	 *  Sets the STUN server to be used for incoming SIP calls that cannot be
	 *  matched to any configured user account.  If \p StunId is INVALID_HANDLE
	 *  the STUN usage will be disabled for these calls.
	 *
	 *  Note that this does not set the STUN server for all user accounts that
	 *  do not have STUN assigned.
	 *
	 *  \param[in] StunId The STUN server to use for unknown incoming calls
	 */
	LIBRESULT (* SetDefaultStunServer )( StunHandler StunId );

	/** \brief Configures the address of the STUN server
	 *
	 *  Use this function to set the address (hostname) of the STUN server.
	 *  Must be called before the server can be used.
	 *
	 *  \param[in] StunId The STUN server to configure
	 *  \param[in] pStunServer Hostname of the server
	 */
	LIBRESULT (* SetStunServer )( StunHandler StunId, const char * pStunServer );

	/** \brief Configures the port of the STUN server
	 *
	 *  Use this function to set the port of the STUN server.  The default
	 *  port is 3478.
	 *
	 *  \param[in] StunId The STUN server to configure
	 *  \param[in] Port The port of the server
	 */
	LIBRESULT (* SetStunPort )( StunHandler StunId, WORD Port );

	/** \brief Sets the refresh period of the STUN server
	 *
	 *  Use this function to specify how often to refresh the STUN server.
	 *  The default is 30 seconds.  The refresh can be used to keep the
	 *  NAT mapping alive.
	 *
	 *  \param[in] StunId The STUN server to configure
	 *  \param[in] ms The refresh period in milliseconds (30000 default)
	 *
	 *  \sa SetUserKeepAliveTime()
	 */
	LIBRESULT (* SetStunRefreshPeriod )( StunHandler StunId, long ms );

	/** \brief Starts the STUN resolve and refresh loop
	 *
	 *  If the server is properly configured (the address is set at least)
	 *  this will initiate the discovery and refresh of the server.  The
	 *  function will return immediately.
	 *
	 *  \param[in] StunId The STUN server to start
	 *
	 *  \sa WrapperCallbacks::onStunNetworkDiscovered()
	 */
	LIBRESULT (* StartStunResolve )( StunHandler StunId );

	/** \brief Stops the STUN resolving and refresh loop
	 *
	 *  The STUN server discovery and refresh will be stopped immediately.
	 *
	 *  \param[in] StunId The STUN server to stop
	 */
	LIBRESULT (* StopStunResolve )( StunHandler StunId );

	/** \brief Gets the STUN server's configured address
	 *
	 *  Retrieve the STUN server's configured address.
	 *
	 *  \return The STUN server address.  The buffer is in the library's
	 *          memory and must not be altered or freed.
	 *
	 *  \sa SetStunServer()
	 */
	const char * (* GetStunServer )( StunHandler StunId );

	/** \brief Gets the STUN server's configured port
	 *
	 *  Retrieve the STUN server's configured port.
	 *
	 *  \return The STUN server port
	 *
	 *  \sa SetStunPort()
	 */
	WORD (* GetStunPort )( StunHandler StunId );

	/** \brief Gets the external SIP address resolved using STUN
	 *
	 *  Informational only.  If the address has been resolved the function will
	 *  return it in network byte order otherwise it will return (DWORD)-1
	 *
	 *  \param[in] StunId The STUN server to check
	 *
	 *  \return The resolved external address
	 */
	DWORD (* GetStunResolvedAddress )( StunHandler StunId );

	/** \brief Gets the external SIP port resolved using STUN
	 *
	 *  Informational only.  If the address has been resolved the function will
	 *  return it otherwise it will return 0.
	 *
	 *  \param[in] StunId The STUN server to check
	 *
	 *  \return The resolved port
	 */
	WORD (* GetStunResolvedPort )( StunHandler StunId );

	/** \brief Gets the discovered network type
	 *
	 *  If the network was discovered it will return the type otherwise
	 *  it will return E_NETWORK_UNKNOWN.
	 *
	 *  \param[in] StunId The STUN server to check
	 *
	 *  \sa WrapperCallbacks::onStunNetworkDiscovered()
	 */
	eNetworkTypeEnum_t (* GetStunNetworkType )( StunHandler StunId );

	/** \brief Checks if the STUN is started
	 *
	 *  Checks if the STUN has been started.
	 *
	 *  \return 0 if the STUN is stopped, 1 if the STUN is started
	 */
	int (* IsStunEnabled )( StunHandler StunId );

	/** \brief Checks if the STUN has resolved the external address
	 *
	 *  Checks if the STUN has resolved the external address.
	 *
	 *  \return 0 if the address is not yet resolved, 1 if the address is resolved
	 */
	int (* IsStunResolved )( StunHandler StunId );

	/** \brief Prepares a port using STUN for a call
	 *
	 *  By default the STUN is used for the SIP signalling, running on
	 *  the SIP UDP port.  This function should be used to prepare the
	 *  RTP UDP port to be used for audio or video.
	 *
	 *  For audio calls you should call this function once.  For video
	 *  calls the function should be called two times.
	 *
	 *  When the port is ready the WrapperCallbacks::onStunPortReady
	 *  event will be generated.
	 *
	 *  \param[in] UserId The account for which to prepare a port.
	 *             The account must have a configured and discovered
	 *             STUN server.
	 *  \param[in] UserData An arbitrary value that will be returned
	 *             with the callback.
	 *
	 *  \sa WrapperCallbacks::onStunPortReady()
	 */
	LIBRESULT (* PrepareStunPort )( UserHandler UserId, void * UserData );

	/** @} */

	/** \addtogroup sound Sound management functions
	 *  @{
	 */

	/** \brief Adds a sound to the sound list
	 *
	 *  Creates a sound structure and copies the provided samples into its buffers.
	 *  If the function is succesful the sound can later be played with StartSound().
	 *
	 *  \param[in] pData The samples to copy
	 *  \param[in] dwLen Size of \p pData in bytes
	 *  \param[in] ucSampleLen Size of a sample in bytes (must be 2)
	 *  \param[in] wFrequency Frequency of the sound in Hz (must be 8000)
	 *  \param[in] bRepeat 1 if the sound should be looped when played
	 *  \param[in] wPauseLen If the sound is to be looped, the amount of milliseconds
	 *             to produce silence between each loop.
	 *
	 *  \return The newly created sound's id or INVALID_HANDLE on error.
	 *
	 *  \sa StartSound(), StopSound(), RemoveSound()
	 */
	SoundHandler (* AddSound )( void * pData, DWORD dwLen, BYTE  ucSampleLen, WORD  wFrequency, int  bRepeat, WORD  wPauseLen );

	/** \brief Start playback of a sound
	 *
	 *  Start playing the sound on one of the two output devices (either the
	 *  selected output device or the selected ringing device).  Use
	 *  SetCurrentOutputDevice() and SetCurrentRingDevice() to select the
	 *  output devices.  The sound will played on the device and will not
	 *  be sent over the communication channel (see StartPlayback()).
	 *
	 *  \param[in] SndId The sound to be played
	 *  \param[in] DeviceId 0 for output, 1 for ringing device
	 *
	 *  \sa SetCurrentOutputDevice(), SetCurrentRingDevice(), AddSound(),
	 *      StopSound(), StartPlayback()
	 */
	LIBRESULT (* StartSound )( SoundHandler SndId, BYTE DeviceId );

	/** \brief Stops playback of a sound
	 *
	 *  Stops playing the sound.  Must be called with the same parameters
	 *  as StartSound().  To stop a sound on both devices you can call:
	 *
	 *  \code
	 *  StopSound( SndId, 0 ); StopSound( SndId, 1 );
	 *  \endcode
	 *
	 *  \param[in] SndId Sound to stop playing
	 *  \param[in] DeviceId 0 for output, 1 for ringing device
	 *
	 *  \sa StartSound()
	 */
	LIBRESULT (* StopSound )( SoundHandler SndId, BYTE DeviceId );

	/** \brief Destroys a sound
	 *
	 *  Destroys the sound's structures and frees the allocated memory.
	 *
	 *  \param[in] SndId The sound to destroy
	 *
	 *  \sa AddSound()
	 */
	LIBRESULT (* RemoveSound )( SoundHandler SndId );

	/** @} */

	/** \addtogroup volume Volume control and energy measuring functions
	 *  @{
	 */

	/** \brief Sets the energy measurement threshold
	 *
	 *  Controls on how much change in the energy levels to react (send
	 *  the level callback).  Default is 0.05.
	 *
	 *  \param[in] delta The minimum change in level between notifications
	 *
	 *  \sa GetAudioDeltaLevel()
	 */
	void (* SetAudioDeltaLevel )( double delta );

	/** \brief Gets the energy measurement threshold
	 *
	 *  Retrieves the current energy measurement threshold.
	 *
	 *  \return The minimum change in level between notifications
	 *
	 *  \sa SetAudioDeltaLevel()
	 */
	double (* GetAudioDeltaLevel )( void );

	/** \brief Sets input volume
	 *
	 *  Sets the input device volume, between 0.0 (muted) and 1.0 (maximum volume).
	 *
	 *  \param[in] level The audio level between 0.0 and 1.0
	 *
	 *  \sa SetAudioOutputLevel(), SetAudioRingingLevel()
	 */
	LIBRESULT (* SetAudioInputLevel )( double level );

	/** \brief Sets output volume
	 *
	 *  Sets the output device volume, between 0.0 (muted) and 1.0 (maximum volume).
	 *
	 *  \param[in] level The audio level between 0.0 and 1.0
	 *
	 *  \sa SetAudioInputLevel(), SetAudioRingingLevel()
	 */
	LIBRESULT (* SetAudioOutputLevel )( double level );

	/** \brief Controls the microphone boost option
	 *
	 *  Enables or disables the microphone boost.  On some platforms the boost
	 *  option is controlled via a text field and this might not work properly.
	 *
	 *  \param[in] boost 1 to enable, 0 to disable
	 *
	 *  \sa GetMicBoost()
	 */
	LIBRESULT (* SetMicBoost )( int boost );

	/** \brief Gets tue current input volume
	 *
	 *  Returns the volume setting for the input device.  Values are between
	 *  0.0 and 1.0.  This is not the measured energy of the input samples.
	 *
	 *  \return The volume setting for the input device between 0.0 and 1.0
	 *
	 *  \sa SetAudioOutputLevel()
	 */
	double (* GetAudioInputLevel )( void );

	/** \brief Gets the current output volume
	 *
	 *  Returns the volume setting for the output device.  Values are between
	 *  0.0 and 1.0.  This is not the measured energy of the output samples.
	 *
	 *  \return The volume setting for the output device between 0.0 and 1.0
	 *
	 *  \sa SetAudioInputLevel()
	 */
	double (* GetAudioOutputLevel )( void );

	/** \brief Retrieves the status of the microphone boost option
	 *
	 *  Retrieves the input device's micrphone boost setting. 
	 *
	 *  \return 0 boost is disabled, 1 if enabled
	 *
	 *  \sa SetMicBoost()
	 */
	int (* GetMicBoost )( void );

	/** @} */

	/** \brief Controls the acoustic echo cancellation
	 *
	 *  \ingroup audio
	 *
	 *  Enables or disabled the Acoustic Echo Cancellation engine. Default is
	 *  disabled.
	 *
	 *  \param[in] AECEnabled 0 to disable, 1 to enable
	 */
	LIBRESULT (* UseEchoCancellation )( int AECEnabled );

	/** \addtogroup conference Conference management functions
	 *  @{
	 */

	/** \brief Create a conference
	 *
	 *  Creates an empty conference.
	 *
	 *  \param[in] InDeviceId Reserved. Set to 0
	 *  \param[in] OutDeviceId Reserved. Set to 0
	 *
	 *  \return The new conference id
	 *
	 *  \sa DestroyConference(), StartConference(), StopConference(),
	 *      HoldConference(), UnholdConference(), JoinCallToConference(),
	 *      LeaveCallFromConference(), MuteConferenceParticipant(),
	 *      UnmuteConferenceParticipant()
	 */
	ConferenceHandler (* CreateConference )( BYTE InDeviceId, BYTE OutDeviceId );

	/** \brief Destroy a conference
	 *
	 *  Destroy an empty conference.
	 *
	 *  \param[in] ConferenceId The id of the conference to destroy
	 *
	 *  \sa CreateConference()
	 */
	LIBRESULT (* DestroyConference )( ConferenceHandler ConferenceId );

	/** \brief Start a conference
	 *
	 *  Starts mixing and multiplexing the calls in the conference.  Use
	 *  StopConference() to stop mixing.
	 *
	 *  \param[in] ConferenceId The conference to start
	 *
	 *  \sa CreateConference(), StopConference(), HoldConference()
	 *      JoinCallToConference(), MuteConferenceParticipant()
	 */
	LIBRESULT (* StartConference )( ConferenceHandler ConferenceId );

	/** \brief Stops a conference
	 *
	 *  Stops mixing the conference.  If you need to pause the conference
	 *  while it has active calls it is recommended to use HoldConference().
	 *
	 *  \param[in] ConferenceId The conference to stop
	 *
	 *  \sa StartConference()
	 */
	LIBRESULT (* StopConference )( ConferenceHandler ConferenceId );

	/** \brief Holds a conference
	 *
	 *  This will put all calls in the conference on hold and stop mixing.
	 *  This is a heavy operation.
	 *
	 *  \param[in] ConferenceId The conference to put on hold
	 *
	 *  \sa UnholdConference()
	 */
	LIBRESULT (* HoldConference )( ConferenceHandler ConferenceId );

	/** \brief Retrieves a conference
	 *
	 *  This will retrieve all the calls in the conference and restart it.
	 *
	 *  \param[in] ConferenceId The conference to retrieve
	 *
	 *  \sa StartConference()
	 */
	LIBRESULT (* UnholdConference )( ConferenceHandler ConferenceId );

	/** \brief Adds a call to the conference
	 *
	 *  Adds a call to the conference.
	 *
	 *  \param[in] ConferenceId The conference
	 *  \param[in] CallId The call to add to the conference
	 *
	 *  \sa LeaveCallFromConference(), MuteConferenceParticipant(),
	 *      StartConference()
	 */
	LIBRESULT (* JoinCallToConference )( ConferenceHandler ConferenceId, CallHandler CallId );

	/** \brief Removes a call from the conference
	 *
	 *  \param[in] ConferenceId The conference
	 *  \param[in] CallId The call to remove
	 *
	 *  \sa JoinCallToConference(), MuteConferenceParticipant(),
	 *      StartConference()
	 */
	LIBRESULT (* LeaveCallFromConference )( ConferenceHandler ConferenceId, CallHandler CallId );

	/** \brief Mutes a call in the conference
	 *
	 *  Will mute all sound coming in and out of the call.
	 *
	 *  \param[in] ConferenceId The conference
	 *  \param[in] CallId The call to mute
	 *
	 *  \sa UnmuteConferenceParticipant(), JoinCallToConference(),
	 *      HoldConference(), CallHold()
	 */
	LIBRESULT (* MuteConferenceParticipant )( ConferenceHandler ConferenceId, CallHandler CallId );

	/** \brief Unmutes a call in the conference
	 *
	 *  Will unmute the sound coming in and out of the call
	 *
	 *  \param[in] ConferenceId The conference
	 *  \param[in] CallId The call to unmute
	 *
	 *  \sa MuteConferenceParticipant(), JoinCallToConference(), CallUnhold()
	 */
	LIBRESULT (* UnmuteConferenceParticipant )( ConferenceHandler ConferenceId, CallHandler CallId );

	/** @} */

	/** \deprecated Reserved for future use
	 */
	LIBRESULT (* ChangeConferenceOutputDevice )( ConferenceHandler ConferenceId, BYTE OutputDevice );

	/** \deprecated Reserved for future use
	 */
	LIBRESULT (* ChangeConferenceInputDevice )( ConferenceHandler ConferenceId, BYTE InputDevice );

	/** \brief Checks if the call is in the conference
	 *
	 *  \ingroup conference
	 *
	 *  \param[in] CallId Call to look up
	 *  \param[in] ConferenceId The conference to check
	 *
	 *  \return 1 if the call is part of the conference, 0 otherwise
	 */
	int (* IsCallInConference )( CallHandler CallId, ConferenceHandler ConferenceId );

	/** \brief Gets the number of calls in a conference
	 *
	 *  \ingroup conference
	 *
	 *  \param[in] ConferenceId Conference to check
	 *  \param[out] pCount Pointer to a byte where to put the number
	 */
	LIBRESULT (* GetNumberOfCallsInConference )( ConferenceHandler ConferenceId, BYTE * pCount );

	/** \brief Gets the actual SIP port
	 *
	 *  \ingroup global
	 *
	 *  In case the requested port was already taken the library will open the
	 *  next available one.
	 *
	 *  \return The actual open SIP UDP port
	 */
	int (* GetSipPort )( void );

	/** \brief Enables the clock skew compensating engine
	 *
	 *  \ingroup audio
	 *
	 *  This function is obsolete.  The clock skew compensation is not needed in
	 *  most cases.
	 */
	LIBRESULT (* UseAudioCompensator )( int CompensatorEnabled );

	/** \addtogroup extec External echo canceller interface
	 *
	 *  The external echo canceller interface is a set of callbacks that
	 *  are connected directly to the audio processing engine and act like
	 *  audio filters.
	 *
	 *  They are only used in a special build of the library and work in
	 *  a fixed sampling rate of 48000Hz.
	 *
	 *  @{
	 */

	/** \brief Sets the initialization callback
	 *
	 *  \param[in] pCbk The initialization callback
	 */
	LIBRESULT (* SetInitExtECCbk )( pfInitExtECCbk pCbk );

	/** \brief Sets the version callback
	 *
	 *  \param[in] pCbk The version callback
	 */
	LIBRESULT (* SetGetExtECVersionCbk )( pfGetExtECVersionCbk pCbk );

	/** \brief Sets the output filter callback
	 *
	 *  \param[in] pCbk The output filter callback
	 */
	LIBRESULT (* SetExtECProcessSpkrBlockCbk )( pfExtECProcessSpkrBlockCbk pCbk );

	/** \brief Sets the input filter callback
	 *
	 *  \param[in] pCbk The input filter callback
	 */
	LIBRESULT (* SetExtECProcessMicBlockCbk )( pfExtECProcessMicBlockCbk pCbk );

	/** \brief Sets the finalization callback
	 *
	 *  \param[in] pCbk The finalization callback
	 */
	LIBRESULT (* SetCloseExtECCbk )( pfCloseExtECCbk pCbk );

	/** @} */

	/** \deprecated The echo canceller will use automatic gain control
	 */
	LIBRESULT (* SetAECNearMeanDB )( float DB );

	/** \deprecated The echo canceller will use automatic gain control
	 */
	LIBRESULT (* SetAECFarMeanDB )( float DB );

	/** \deprecated The echo canceller will use automatic gain control
	 */
	LIBRESULT (* SetAECSingleMeanDB )( float DB );

	/** \brief Reset the audio driver
	 *
	 *  \ingroup audio
	 *
	 *  Use this function to reset the audio driver.  The driver can be
	 *  reset anytime (during a call too) but there could be temporary
	 *  stuttering and click in the sound.
	 *
	 *  This function is necessary to detect if new audio devices has
	 *  appeared (or old devices disappeared).  The library will try to
	 *  re-open the old devices (if they were open during the reset) but
	 *  if the devices were gone it will choose the current system
	 *  defaults.
	 *
	 *  Re-enumerating the audio devices is required after this call.
	 */
	LIBRESULT (* ResetAudioDriver )( void );

	/** \deprecated The plain text authentication is a security risk
	 *
	 *  \param[in] userId User id
	 *  \param[in] plaintextEnabled 1 to enable plain text authentication
	 */
	LIBRESULT (* SetIaxUserPlaintextSupport )( UserHandler userId, int plaintextEnabled );

	/** \brief Sets the SIP TCP port to open
	 *
	 *  \ingroup init
	 *
	 *  Must be called before InitCallManager().  If the requested port is
	 *  not available the next free port will be used.  Use the value of 0
	 *  to try to open the same as UDP; Use the value of -1 to disable the
	 *  opening of a TCP socket (default).
	 *
	 *  \param[in] port The TCP port to open
	 *
	 *  \sa InitCallManager(), SetSipTlsPort()
	 */
	LIBRESULT (* SetSipTcpPort )( int port );

	/** \brief Sets the SIP TLS port to open
	 *
	 *  \ingroup init
	 *
	 *  Must be called before InitCallManager(). If the requested port is
	 *  not available the next free port will be used.  Use the value of 0
	 *  to try to open the next port after the TCP port (if enabled).  Use
	 *  the value of -1 to disable the TLS socket (default).
	 *
	 *  \param[in] port The TLS port to open
	 *
	 *  \sa InitCallManager(), SetSipTcpPort()
	 */
	LIBRESULT (* SetSipTlsPort )( int port );

	/** \brief Open the debug log and start logging
	 *
	 *  \ingroup init
	 *
	 *  Opens the file for debug logging.  Can be called at any time.  The
	 *  file will be overwritten.  The file name is limited to ASCII
	 *  characters on Windows and UTF-8 on linux.
	 *
	 *  The log file will contain mainly entries from the SIP stack but
	 *  SipDebugLog() can be used to log arbitrary messages at any time.
	 *
	 *  \param[in] pszFilename The file name
	 *
	 *  \sa StopResipLog(), SipDebugLog()
	 */
	LIBRESULT (* StartResipLog )( const char * pszFilename );

	/** \brief Stops logging
	 *
	 *  \ingroup init
	 *
	 *  \sa StartResipLog()
	 */
	LIBRESULT (* StopResipLog )( void );

	/** \addtogroup fax Fax and image management functions
	 *  @{
	 */

	/** \brief Set SIP T.38 support
	 *
	 *  The default is disabled.
	 *
	 *  \param[in] enabled 1 to enable, 0 to disable t.38 (fax support)
	 */
	LIBRESULT (* SetSipFaxSupport )( int enabled );

	/** \brief Set the user's CSI
	 *
	 *  The CSI (Called Subscriber Identification) is a telephone number that is
	 *  transmitted during the fax negotiation.  Can contain numbers and spaces and
	 *  can optionally start with '+'.  Maximum 20 symbols.
	 *
	 *  All faxes sent from this user will be from this CSI
	 *
	 *  \param[in] UserId User to configure
	 *  \param[in] pszFaxId CSI to set for this user
	 *
	 *  \sa CallCreateFax(), FaxImageLoad()
	 */
	LIBRESULT (* SetUserFaxId )( UserHandler UserId, const char * pszFaxId );

	/** \brief Accepts an incoming t.38 fax offer
	 *
	 *  Call only if the call received a t.38 offer (the WrapperCallbacks::onFaxIncomingOffer
	 *  callback has fired).  This function will switch the call into t.38 receiving mode.
	 *
	 *  \param[in] CallId Call for which we've got the t.38 offer
	 *  \param[in] pszFilename Where to save the incoming fax
	 *  \param[in] mode Fax writer mode
	 *
	 *  \sa FaxReject(), WrapperCallbacks::onFaxIncomingOffer(),
	 *      WrapperCallbacks::onFaxRemoteId(), WrapperCallbacks::onFaxStarted(),
	 *      WrapperCallbacks::onFaxPage(), WrapperCallbacks::onFaxError(),
	 *      WrapperCallbacks::onFaxDone()
	 */
	LIBRESULT (* FaxAccept )( CallHandler CallId, const char * pszFilename, eFaxWriterMode_t mode );

	/** \brief Rejects an incoming t.38 fax offer
	 *
	 *  Call only if the call has received a t.38 offer (the WrapperCallbacks::onFaxIncomingOffer
	 *  callback has fired).  This function will reject the offer.  The call will continue to
	 *  work as usual.
	 *
	 *  \param[in] CallId The call for which we've got the t.38 offer
	 *
	 *  \sa FaxAccept(), WrapperCallbacks::onFaxIncomingOffer()
	 */
	LIBRESULT (* FaxReject )( CallHandler CallId );

	/** \brief Creates a call for sending fax over t.38
	 *
	 *  Works only with SIP users.
	 *
	 *  Acts initially like CallCreate().  After (and if) the call is answered from the remote
	 *  end, it will be re-invited for fax (t.38).  If the remote end accepts the t.38 offer
	 *  the call will switch to fax transmitting mode.
	 *
	 *  Use FaxImageLoad() to load arbitrary TIFF file and convert it to a fax image.
	 *
	 *  If this functions succeeds it will take ownership of the fax image.  It must not be
	 *  used in other calls and must not be destroyed (the call will automatically destroy
	 *  it when it does not need it anymore).
	 *
	 *  The function will return immediately.  If successful, the call will be started and
	 *  processed in a separate thread.
	 *
	 *  \param[in] UserId User from which the call to originate
	 *  \param[in] pszCallee Number to call
	 *  \param[in] ImageId Image to send (FaxImageLoad())
	 *  \param[out] pCallId Pointer to call handler where the new call id will be stored
	 *
	 *  \sa FaxImageLoad(), CallCreate(), WrapperCallbacks::onFaxOutgoingOffer(),
	 *      WrapperCallbacks::onFaxStarted(), WrapperCallbacks::onFaxPage(),
	 *      WrapperCallbacks::onFaxError(), WrapperCallbacks::onFaxDone()
	 */
	LIBRESULT (* CallCreateFax )( UserHandler UserId, const char * pszCallee, ImageHandler ImageId, CallHandler * pCallId );

	/** \brief Prepares a TIFF file for t.38 transmission
	 *
	 *  The supported image compressions are: LZW, G3, G4, JPEG, Deflate, None (uncompressed).
	 *  Each page of the image is resampled to the requested DPI and cropped/filled to fit
	 *  horizontally on a A4/Letter/Legal page of the specified resolution.  Vertically it
	 *  is only cropped but not filled.
	 *
	 *  200x200 DPI: 1728 x 2286 (the only resolution supported for now - Fax "Fine" resolution)
	 *
	 *  During the image conversion WrapperCallbacks::onFaxImgProcess() will be called to
	 *  indicate the current progress and the total progress for this image.
	 *
	 *  After it is processed, WrapperCallback::onFaxImgLoaded() will be called to indicate
	 *  if the conversion was successful.  You must wait for this callback and it must report
	 *  success before you can use the image in CallCreateFax().
	 *
	 *  \param[in] pszFilename File name of the TIFF image to process
	 *  \param[in] xRes Horizontal resolution. Must be 200
	 *  \param[in] yRes Vertial resolution. Msut be 200
	 *  \param[out] pImageId Pointer to image handler, will be filled with the id of the new
	 *              image
	 *  \param[in] mode Image processing mode
	 *
	 *  \sa WrapperCallbacks::onFaxImgProcess(), WrapperCallback::onFaxImgLoaded(),
	 *      CallCreateFax(), FaxImageDestroy()
	 */
	LIBRESULT (* FaxImageLoad )( const char * pszFilename, float xRes, float yRes, ImageHandler * pImageId, eFaxWriterMode_t mode );

	/** \brief Destroy a fax image
	 *
	 *  Use this function to cancel processing or destroy a fax image if CallCreateFax()
	 *  was never used on it.  If this function is being used to cancel the loading of
	 *  an image, the WrapperCallbacks::onFaxImgLoaded() will be called to indicate that
	 *  the loading has failed.
	 *
	 *  \param[in] ImageId The image to cancel/destroy
	 *
	 *  \sa FaxImageLoad(), CallCreateFax()
	 */
	LIBRESULT (* FaxImageDestroy )( ImageHandler ImageId );

	/** @} */

	/** \brief Log a message to the debug log
	 *
	 *  \ingroup global
	 *
	 *  \param[in] pszMessage The message to log in the UTF-8 character set.
	 *
	 *  \sa StartResipLog()
	 */
	LIBRESULT (* SipDebugLog )( const char * pszMessage );

	/** \brief Accept a call transfer request
	 *
	 *  \ingroup call
	 *
	 *  Must be called only if WrapperCallbacks::onCallTransferStarted() has fired.
	 *  This is usually a blind (unattended) transfer request.  The request will be
	 *  accepted and a new call will be initiated.  Periodical updates on the new
	 *  call's state will be sent to the old call.  The old call will be hung up
	 *  only after the new call is succesful.
	 *
	 *  The new call will get all the relevant callbacks as if it was created by
	 *  using CallCreate().
	 *
	 *  \param[in] CallId The call for which we got a transfer request
	 *  \param[out] pNewCallId The new call that will attempt to connect to the
	 *              number in the request
	 *
	 *  \sa TransferReject(), WrapperCallbacks::onCallTransferStarted(),
	 *      WrapperCallbacks::onCallTransferSucceeded(),
	 *      WrapperCallbacks::onCallTransferFailure(), CallCreate()
	 */
	LIBRESULT (* TransferAccept )( CallHandler CallId, CallHandler * pNewCallId );

	/** \brief Rejects a call transfer request
	 *
	 *  \ingroup call
	 *
	 *  Must be called only if WrapperCallbacks::onCallTransferStarted() has fired.
	 *  Use the \p causeCode parameter to reject the transfer with a specific reason.
	 *  For SIP, the call will continue as usual.
	 *
	 *  \param[in] CallId The call that got a transfer request
	 *  \param[in] causeCode Cause code with which to reject the transfer request
	 *
	 *  \sa TransferAccept(), WrapperCallbacks::onCallTransferStarted()
	 */
	LIBRESULT (* TransferReject )( CallHandler CallId, int causeCode );

	/** \addtogroup blf Busy Lamp Field management functions
	 *  @{
	 */

	/** \brief Add a SIP BLF peer
	 *
	 *  Adds the peer to the user's BLF monitoring list.  The peers must be added before
	 *  the user is registered.  Peer dialog subscription will be done automatically on
	 *  user registration.
	 *
	 *  \param[in] UserId The user which list to modify
	 *  \param[in] peerName Name of the peer to monitor (display name)
	 *  \param[in] peerNumber Number of the peer to monitor (user name/phone nubmer)
	 *  \param[out] PeerId The resulting peer id will be put here on success
	 *
	 *  \sa RemovePeer(), RegisterUser(), WrapperCallbacks::onPeerDialogAdded(),
	 *      WrapperCallbacks::onPeerDialogChanged(), WrapperCallbacks::onPeerDialogRemoved()
	 */
	LIBRESULT (* AddPeer )( UserHandler UserId, const char * peerName, const char * peerNumber, PeerHandler * PeerId );

	/** \brief Remove a SIP BLF peer
	 *
	 *  Removes the peer from the user's BLF monitoring list and immediately unsubscribes
	 *  for its dialog notifications.
	 *
	 *  \param[in] PeerId The peer to stop monitoring
	 *
	 *  \sa AddPeer()
	 */
	LIBRESULT (* RemovePeer )( PeerHandler PeerId );

	/** @} */

	/** \brief Select the DTMF band for the user
	 *
	 *  \ingroup user
	 *
	 *  \param[in] UserId The user to configure
	 *  \param[in] DtmfBand The DTMF band to select
	 *
	 *  \sa AddUser()
	 */
	LIBRESULT (* SetUserDtmfBand )( UserHandler UserId, eDtmfBand_t DtmfBand );

	/** \brief Select the user's SRTP mode (only for SIP)
	 *
	 *  \ingroup user
	 *
	 *  Only E_SRTP_SDES is supported for now.  Use E_SRTP_NONE to disable
	 *  SRTP.
	 *
	 *  \param[in] UserId The user to configure
	 *  \param[in] SrtpNeg The SRTP mode
	 */
	LIBRESULT (* SetUserSrtp )( UserHandler UserId, eSrtpNegotiation_t SrtpNeg );

	/** \brief Select DSCP for the media streams
	 *
	 *  \ingroup init
	 *
	 *  Call this function before InitCallManager().  This will affect the
	 *  RTP, SRTP and IAX2 sockets.  Might require administrative privileges.
	 *
	 *  \param[in] dscp The DiffServ Code Point
	 *
	 *  \sa SetSignalDSCP(), InitCallManager()
	 */
	LIBRESULT (* SetMediaDSCP )( int dscp );

	/** \brief Select DSCP for the signaling streams
	 *
	 *  \ingroup init
	 *
	 *  Call this function before InitCallManager().  This will affect the
	 *  SIP sockets (UDP, TCP and TLS).
	 *
	 *  \param[in] dscp The DiffServ Code Point
	 *
	 *  \sa SetMediaDSCP(), InitCallManager()
	 */
	LIBRESULT (* SetSignalDSCP )( int dscp );

	/** \brief Select the user agent for SIP
	 *
	 *  \ingroup init
	 *
	 *  Used in SIP messages mainly. (User-Agent header field) Best to be
	 *  set before InitCallManager().
	 *
	 *  \param[in] pszUserAgent The string must be UTF-8
	 *
	 *  \sa InitCallManager()
	 */
	LIBRESULT (* SetUserAgent )( const char * pszUserAgent );

	/** \brief Set the user registration refresh time
	 *
	 *  \ingroup user
	 *
	 *  The default registration refresh is 70 seconds.  Use this function to
	 *  overwrite the setting.  Best to use it before RegisterUser().  Note that
	 *  the server might enforce different (shorter) refresh time.  The stack
	 *  will not wait for the full period to refresh the registration.  It will
	 *  try to refresh it after 90% of the negotiated time has elapsed.
	 *
	 *  \param[in] UserId The user to configure
	 *  \param[in] seconds The registration refresh period in seconds
	 *
	 *  \sa AddUser(), RegisterUser()
	 */
	LIBRESULT (* SetUserRegistrationTime )( UserHandler UserId, int seconds );

	/** \brief Request Messages Waiting notifications
	 *
	 *  \ingroup user
	 *
	 *  Some servers (asterisk) will send notifications regardless of subscriptions.
	 *  For IAX2 the information is always sent.  Use this to explicitly request MWI
	 *  notifications for a SIP user.  Using this for IAX2 is harmless (it will return
	 *  an error though).
	 *
	 *  The function can be used at any time (relative to the RegisterUser() call).
	 *  It can also be used for users that are not registered (though some servers
	 *  can have serious issues with that).
	 *
	 *  There is a known issue with Asterisk if this function is called after registration
	 *  (it is a non-fatal issue that can delay the MWI notification).
	 *
	 *  WrapperCallbacks::onMessagesWaiting() will be called when a MWI notification
	 *  is received for this user.
	 *
	 *  \param[in] UserId The user to request MWI for
	 *  \param[in] subscriptionSeconds The subscription refresh period in seconds
	 *
	 *  \sa AddUser(), RegisterUser(), WrapperCallbacks::onMessagesWaiting()
	 */
	LIBRESULT (* RequestMessagesWaiting )( UserHandler UserId, int subscriptionSeconds );

	/** \brief Retrieve the value of the DOCUMENTANME tag in a TIFF file
	 *
	 *  \ingroup fax
	 *
	 *  Only valid for TIFF files.  This call can also be used to check the structural
	 *  validity of a TIFF file, but not if the image compression inside is supported.
	 *
	 *  The function will also count the number of pages in the TIFF document. The
	 *  function is meant to be used in conjuction with the Zoiper Print to Fax driver.
	 *
	 *  \param[in] fileName The file name of the TIFF image to process
	 *  \param[out] docNameBuf Buffer for the document name (copied from DOCUMENTNAME).  The character
	 *              set depends on the TIFF creator.
	 *  \param[in] docNameBufSize The size of the \p docNameBuf buffer in bytes
	 *  \param[out] numPages The number of pages in the document
	 *
	 *  \sa FaxImageLoad()
	 */
	LIBRESULT (* GetFaxDocumentInfo )( const char * fileName, char * docNameBuf, int docNameBufSize, int * numPages );

	/** \addtogroup video Video management functions
	 *  @{
	 */

	/** \brief Send a video frame over the network
	 *
	 *  Send a YUV 420p frame to the video processing thread identified by \p pUserData (returned
	 *  by WrapperCallbacks::onVideoStarted()).
	 *
	 *  The function will copy the buffer into its internal strutures and schedule
	 *  it for encoding and transmitting over the video call.  It will return immediately.
	 *
	 *  \param[in] pThreadId The thread id send by WrapperCallbacks::onVideoStarted()
	 *  \param[in] pBuffer The buffer with the YUV 420p frame
	 *  \param[in] bufLen The size of the buffer in bytes
	 *
	 *  \sa WrapperCallbacks::onVideoStarted(), CallCreateVideo()
	 */
	LIBRESULT (* VideoSendFrame )( void * pThreadId, const void * pBuffer, int bufLen );

	/** \brief Create a video call
	 *
	 *  Creates a call with both video and audio offered.  The video codecs must be added
	 *  by using either AddCodec() or AddUserCodec().
	 *
	 *  The other semantics are the same as CallCreate()
	 *
	 *  \param[in] userId The user from which to originate the video offer
	 *  \param[in] pCallee The user/number to call
	 *  \param[out] CallId The resulting id of the new call, if successful
	 *
	 *  \sa CallCreate()
	 */
	LIBRESULT (* CallCreateVideo )( UserHandler userId, const char * pCallee, CallHandler * CallId );

	/** \brief Set the video encoder's bitrate
	 *
	 *  Sets the video encoder's output bitrate in bits per second.  This function along
	 *  with AddVideoFormat() will affect all video calls.  Because the frames come outside
	 *  of this library if the frame rate is not as configured the resulting bit rate can
	 *  differ greatly.  Example: if the library's video encoder is configured for 128000
	 *  bps for a video format of 352x288 and 5 fps, but the frames actually come at 10fps
	 *  the resulting bitrate will be 256000 bps.
	 *
	 *  \param[in] bps The bitrate of the encoder in bits per second
	 *
	 *  \sa AddVideoFormat()
	 */
	LIBRESULT (* SetVideoBitrate )( int bps );

	/** \brief Add a video format for the video negotiations
	 *
	 *  Adds the format to the list of supported formats used while negotiating video
	 *  calls.  You can only add one video format at a time in this version of the
	 *  library.  To change the video format use ClearVideoFormats().  Most codecs
	 *  have limitations regarding the frame dimensions.  CIF formats are always
	 *  supported.
	 *
	 *  When the format is negotiated with remote peer, the
	 *  WrapperCallbacks::onVideoFormatSelected() callback will be called.
	 *
	 *  \param[in] width Width of the video frame in pixels
	 *  \param[in] height Height of the video frame in pixels
	 *  \param[in] fps Frames per second of the video stream
	 *
	 *  \sa SetVideoBitrate(), ClearVideoFormats(), WrapperCallbacks::onVideoFormatSelected()
	 */
	LIBRESULT (* AddVideoFormat )( int width, int height, float fps );

	/** \brief Clear the list of video formats
	 *
	 *  \sa AddVideoFormat()
	 */
	LIBRESULT (* ClearVideoFormats )( void );

	/** \brief Sets the video frame callback
	 *
	 *  The callback is invoked from a dedicated video processing thread and is not
	 *  part of the WrapperCallbacks structure.  Use this function to set the callback.
	 *
	 *  The callback is limited to the H.263 (and variants) frame format (YUV 420p
	 *  with alignment).
	 *
	 *  \param[in] pCbk The video frame callback
	 *
	 *  \sa CallCreateVideo()
	 */
	LIBRESULT (* SetVideoFrameIYUVCbk )( pfVideoFrameIYUVCbk pCbk );

	/** \brief Enable video for a call
	 *
	 *  Use this in the following cases:
	 *  \li To enable video during a normal (audio) call
	 *  \li To mark an incoming call as video capable before accepting it
	 *  \li To accept a video offer during a normal (audio) call.
	 *
	 *  It is mandatory to use either CallEnableVideo() or CallDisableVideo()
	 *  for all incoming calls (library limitation).
	 *
	 *  \param[in] CallId The call to operate on
	 *
	 *  \sa CallDisableVideo(), WrapperCallbacks::onVideoOffered()
	 */
	LIBRESULT (* CallEnableVideo )( CallHandler CallId );

	/** \brief Disable video for a call
	 *
	 *  Use this in the following cases:
	 *  \li To disable the video during a call and go back to audio-only
	 *  \li To mark an incoming call as audio-only before accepting it
	 *  \li To reject a video offer during a normal (audio) call.
	 *
	 *  It is mandatory to use either CallEnableVideo() or CallDisableVideo()
	 *  for all incoming calls (library limitation).
	 *
	 *  \param[in] CallId The call to operate on
	 *
	 *  \sa CallEnableVideo(), WrapperCallbacks::onVideoOffered()
	 */
	LIBRESULT (* CallDisableVideo )( CallHandler CallId );

	/** @} */

	/** \addtogroup presence Presence management functions
	 *  @{
	 */

	/** \brief Reject a subscription request
	 *
	 *  \p SubId becomes invalid after this call.  Use this function after the
	 *  WrapperCallbacks::onSubscriptionRequest() callback to reject the offer before
	 *  any updates can be sent or any time after it was accepted with SubscriptionAccept()
	 *  to stop sending updates.
	 *
	 *  Rejection type can be:
	 *  \li \p REJECTION_REJECT means that we stop presence notifications and we don't want more
	 *      subscriptions from this subscriber (though if the subscriber will honor it is another
	 *      question).  \em Hard reject.
	 *  \li \p REJECTION_DEACTIVATED means that we are going offline and will stop notifications
	 *      but the subscriber is welcome to a new subscription when we come back.  \em Soft reject.
	 * 
	 *  \param[in] SubId Subscriber to reject
	 *  \param[in] eRejection Rejection type
	 *  \param[in] pReason Reserved for future use. Pass an empty string
	 *
	 *  \sa SubscriptionAccept(), WrapperCallbacks::onSubscriptionRequest()
	 */
	LIBRESULT (* SubscriptionReject )( SubscriberHandler SubId, eRejectionType_t eRejection, const char * pReason );

	/** \brief Accept or update a subscription
	 *
	 *  Use this function to accept a subscription request from WrapperCallbacks::onSubscriptionRequest()
	 *  and to update your status afterwards.
	 *
	 *  \param[in] SubId Subscription to accept/update
	 *  \param[in] eStatus Our status (sending offline status is ok)
	 *  \param[in] pNote If not NULL this will be encoded in the PIDF document
	 *
	 *  \sa SubscriptionReject(), WrapperCallbacks::onSubscriptionRequest()
	 */
	LIBRESULT (* SubscriptionAccept )( SubscriberHandler SubId, eContactState_t eStatus, const char * pNote );

	/** \brief Add a new contact and subscribe for its presence
	 *
	 *  Only valid for SIP.  The full contact uri is generated from the user's
	 *  address (the contact must be on the same server). The subscription,
	 *  if successful, will be updated each \p subscribeSeconds seconds.
	 *
	 *  Updates about the contact will be delivered via callbacks.
	 *
	 *  \param[in] UserId The user to make the subscription
	 *  \param[in] pContactNumber Username/number of the contact to subscribe for
	 *  \param[out] pContact The resulting contact id
	 *  \param[in] subscribeSeconds The expire time of the subscription
	 *  \param[in] refreshSeconds Reserved, set to -1
	 *
	 *  \sa RemoveContact(), RefreshContact(), SendPlainMessage(),
	 *      WrapperCallbacks::onContactStatus(),
	 *      WrapperCallbacks::onContactTerminated(),
	 *      WrapperCallbacks::onContactRetrying()
	 */
	LIBRESULT (* AddContact )( UserHandler UserId, const char * pContactNumber, ContactHandler * pContact, int subscribeSeconds, int refreshSeconds );

	/** \brief Remove a contact
	 *
	 *  Only valid for SIP.  The subscription for this contact will be terminated
	 *  and the contact destroyed.
	 *
	 *  \param[in] ContactId The contact to remove
	 *
	 *  \sa AddContact()
	 */
	LIBRESULT (* RemoveContact )( ContactHandler ContactId );

	/** \brief Force a refresh for a contact
	 *
	 *  Only valid for SIP.  Note that forcing a refresh could not always work
	 *  depending on the remote end and/or the server configuration.
	 *
	 *  \param[in] ContactId Contact to force refresh
	 *
	 *  \sa AddContact()
	 */
	LIBRESULT (* RefreshContact )( ContactHandler ContactId );

	/** \brief Start a presence publication for the user
	 *
	 *  Only valid for SIP.  Use this function to publish the user's status
	 *  on the server (if the server supports it).  The publication will
	 *  automatically update every \p refreshSeconds seconds.  Use this
	 *  function again at any time to change the user's status.
	 *
	 *  \param[in] UserId The user which status to publish
	 *  \param[in] eStatus Status of the user
	 *  \param[in] pNote If not NULL, this will be published in the PIDF
	 *  \param[in] refreshSeconds Refresh period in seconds
	 *
	 *  \sa CancelPublication(), WrapperCallbacks::onPublicationSucceeded(),
	 *      WrapperCallbacks::onPublicationRetrying(),
	 *      WrapperCallbacks::onPublicationFailed()
	 */
	LIBRESULT (* PublishStatus )( UserHandler UserId, eContactState_t eStatus, const char * pNote, int refreshSeconds );

	/** \brief Stop publishing status for this user
	 *
	 *  Only valid for SIP.
	 *
	 *  \param[in] UserId The user which status to stop publishing
	 *
	 *  \sa PublishStatus()
	 */
	LIBRESULT (* CancelPublication )( UserHandler UserId );

	/** \brief Send a plaintext message (text/plain mime type)
	 *
	 *  Only valid for SIP.  Must have a valid contact.  The generated
	 *  message willb e sent over the signaling channel (SIP) and thus it
	 *  has its limitations on the body size.  The content-type of the
	 *  message will be text/plain.
	 *
	 *  The library will keep track of the message until the transmission
	 *  ends (either succeeds or fails).  There will be a callback to
	 *  confirm the result.
	 *
	 *  \param[in] ContactId Contact to whom to send the message
	 *  \param[in] contentLength Size of the message in bytes
	 *  \param[in] body The message body in UTF-8
	 *  \param[out] pMessageId Will be filled with the message id for the callbacks
	 *
	 *  \sa AddContact(), WrapperCallbacks::onMessageReceived(),
	 *      WrapperCallbacks::onMessageSent(), WrapperCallbacks::onMessageFailed(),
	 *      SetMessageComposingState(), WrapperCallbacks::onContactIsComposing()
	 */
	LIBRESULT (* SendPlainMessage )( ContactHandler ContactId, int contentLength, const char * body, MessageHandler * pMessageId );

	/** \brief Notify a contact that we're composing a message
	 *
	 *  Only valid for SIP.  Updates the message composing state (typing notifications).
	 *  The library can throttle/limit the message composing messages to avoid
	 *  flooding the network.
	 *
	 *  When the user starts typing a message you should send a message
	 *  composing notification with \p active set to 1.  If the user stops
	 *  typing (detected with a timer in the GUI) a composing notification
	 *  with \p active set to 0 should be sent.  When the message is finally
	 *  sent a notification with \p active set to 0 is not needed (it is
	 *  implied by the protocol).
	 *
	 *  \param[in] ContactId The contact to which we're composing a message
	 *  \param[in] active The state of the composition, 0=inactive, 1=active
	 *
	 *  \sa SendPlainMessage(), WrapperCallbacks::onContactIsComposing()
	 */
	LIBRESULT (* SetMessageComposingState )( ContactHandler ContactId, int active );

	/** @} */

	/** \addtogroup soundrec Sound recording management functions
	 *
	 *  The recording functions can be used to record the microphone input
	 *  into sound objects which then can be played back using the \ref sound
	 *  "sound functions".  The recording buffer can also be retrieved by
	 *  the library user.
	 *
	 *  @{
	 */

	/** \brief Create a new recording
	 *
	 *  Creates a new recording object that can take maximum \p maxLengthMS audio
	 *  data in milliseconds.  On success it will return L_OK and the new
	 *  recording Id in \p pRecordingId.  The format is always 8 kHz 16 bit
	 *  slinear (PCM) in host order.
	 *
	 *  \param[in] maxLengthMS Maximum recording length in milliseconds
	 *  \param[out] pRecordingId The resulting recording id
	 *
	 *  \sa StartRecording(), StopRecording(), GetRecordingBuffer(),
	 *      CreateSoundFromRecording(), RemoveRecording()
	 */
	LIBRESULT (* AddRecording )( int maxLengthMS, RecordingHandler * pRecordingId );

	/** \brief Start recording
	 *
	 *  Starts recording into the object with the specified Id.  If the recording
	 *  was already started this will reset the position (will restart it).
	 *  Can be used more than once in the lifetime of a recording object.
	 *
	 *  \param[in] recordingId Id of the recording to start
	 *
	 *  \sa AddRecording(), StopRecording(), WrapperCallbacks::onRecordingFinished(),
	 *      GetRecordingBuffer()
	 */
	LIBRESULT (* StartRecording )( RecordingHandler recordingId );

	/** \brief Stops the recording
	 *
	 *  Has no effect if the recording is not running.  The recording can be
	 *  started again at any time but the position will be reset.
	 *
	 *  \param[in] recordingId The recording to stop
	 *
	 *  \sa StartRecording(), GetRecordingBuffer(),
	 *      WrapperCallbacks::onRecordingFinished()
	 */
	LIBRESULT (* StopRecording )( RecordingHandler recordingId );

	/** \brief Gets the recording data
	 *
	 *  Returns the count of recorded samples and a pointer to the recording's buffer.
	 *  Can return 0 in \p pSampleCount if nothing was yet recorded.  The buffer returned
	 *  in \p ppSamples resides in the library's memory and will be destroyed when
	 *  RemoveRecording() is called.
	 *
	 *  \param[in] recordingId The recording to retrieve
	 *  \param[out] ppSamples Pointer to a pointer to the buffer with samples
	 *  \param[out] pSampleCount Pointer to the count of recorded samples in the buffer
	 *
	 *  \sa StartRecording(), StopRecording()
	 */
	LIBRESULT (* GetRecordingBuffer )( RecordingHandler recordingId, short ** ppSamples, int * pSampleCount );

	/** \brief Creates a sound object from a recording object
	 *
	 *  Gets the recorded samples from a recording object and creates a new sound object
	 *  by copying them.  On success, the new sound handler is put in \p pSoundId.  Use
	 *  the normal \ref sound "Sound functions" on this new handler afterwards.
	 *  The recording object is still valid after this call.
	 *
	 *  \param[in] recordingId The recording to copy into a sound
	 *  \param[out] pSoundId The newly created sound id
	 *
	 *  \sa AddRecording(), AddSound()
	 */
	LIBRESULT (* CreateSoundFromRecording )( RecordingHandler recordingId, SoundHandler * pSoundId );

	/** \brief Remove a recording
	 *
	 *  Remove a recording (stopping it if necessary) and free all data. If
	 *  GetRecordingBuffer() was used, make sure the sample pointer will not be
	 *  dereferenced after this call.  Sound created with CreateSoundFromRecording()
	 *  are safe.
	 *
	 *  \param[in] recordingId The recording to destroy
	 *
	 *  \sa AddRecording()
	 */
	LIBRESULT (* RemoveRecording )( RecordingHandler recordingId );

	/** @} */

	/** \brief Adds certificates for SIP TLS from a PEM file
	 *
	 *  \ingroup global
	 *
	 *  Adds the certificates found in the file.  The file must be in PEM format.
	 *  Note that on Windows platforms the certificate authorities from the system
	 *  certificate store will be automatically added.
	 *
	 *  \param[in] filename File name containing PEM certificates to add
	 *
	 *  \sa AddCertificatesDirect()
	 */
	LIBRESULT (* AddCertificates )( const char * filename );

	/** \brief Adds certificates for SIP TLS from memory
	 *
	 *  \ingroup global
	 *
	 *  Adds the certificates at that memory location.  The format must still be
	 *  PEM like in AddCertificates().
	 *
	 *  \param[in] data Pointer to the buffer containing PEM certificates
	 *  \param[in] dataLen Size of the buffer in bytes
	 *
	 *  \sa AddCertificates()
	 */
	LIBRESULT (* AddCertificatesDirect )( const void * data, int dataLen );

	/** \brief Enables rport for SIP users
	 *
	 *  \ingroup user
	 *
	 *  This function can be used to discover the public address and port in case
	 *  there is a NAT between the user and the server.  It also helps for normal
	 *  unfirewalled TCP and TLS connections (highly recommended for these two
	 *  protocols).
	 *
	 *  If rport is enabled for UDP connects along with STUN, STUN will be preferred.
	 *
	 *  The default is to have rport disabled for UDP.  To enable rport, call this
	 *  function before RegisterUser().  A registration must be done to do a full
	 *  discovery before making any calls if they are to benefit from rport.
	 *
	 *  \param[in] UserId The user to configure
	 *  \param[in] bRport Set to true to enable rport for this user
	 *
	 *  \sa SetUserRportMedia(), AddStunServer()
	 */
	LIBRESULT (* SetUserRport )( UserHandler UserId, int bRport );

	/** \brief Enables rport for RTP
	 *
	 *  \ingroup user
	 *
	 *  Enables usage of rport discovered public address for media negotiations.
	 *  Can help in some firewall/NAT/VPN setups where the port is not changed,
	 *  only the private address is replaced with a public one.  When both rport
	 *  and STUN are enabled, STUN will be preferred.
	 *
	 *  This option is not recommended.  Enable it only if you absolutely know
	 *  what you're doing.
	 *
	 *  \param[in] UserId The user to configure
	 *  \param[in] bRportMedia Set to true to enable rport media
	 *
	 *  \sa SetUserRport(), AddStunServer(), PrepareStunPort()
	 */
	LIBRESULT (* SetUserRportMedia )( UserHandler UserId, int bRportMedia );

	/** \brief Resets the internal asynchronous DNS client
	 *
	 *  \ingroup init
	 *
	 *  Use with care.
	 *
	 */
	LIBRESULT (* ResetDns )( void );

	/** \brief Starts playback over the current call
	 *
	 *  \ingroup sound
	 *
	 *  Starts playing back the sound to the remote peer(s) (if there are
	 *  active calls) instead of the microphone input.  Microphone input will be
	 *  discared during the playback.  Optionally plays the sound on the
	 *  output/ringing device (this is the "monitor" device).  When the whole
	 *  sound has been played out the microphone will be reconnected again.
	 *  All changes made to the microphone during the playback will be applied
	 *  after normal operation has been resumed.
	 *
	 *  \param[in] SoundId The sound to play over the network
	 *  \param[in] monitorDevice Monitoring device
	 *
	 *  \sa StartSound(), StopPlayback()
	 */
	LIBRESULT (* StartPlayback )( SoundHandler SoundId, eOutputDeviceEnum_t monitorDevice );

	/** \brief Stops any playback
	 *
	 *  \ingroup sound
	 *
	 *  Stops any playback and monitored sound immediately if there is such.
	 *
	 *  \sa StartPlayback()
	 */
	LIBRESULT (* StopPlayback )( void );

	/** \brief Adds a custom event to the callback queue
	 *
	 *  \ingroup event
	 *
	 *  This will make the function pointed by \p pCbk to be called with
	 *  its parameter set to \p pUserData the next time PollEvents() is
	 *  called.
	 *
	 *  Use this function to schedule your own events to be executed in
	 *  your event handling thread (from the same thread or a different
	 *  one).
	 *
	 *  \param[in] pCbk Pointer to the function to call
	 *  \param[in] pUserData The parameter of the function
	 *
	 *  \sa PollEvents()
	 */
	LIBRESULT (* AddCustomEvent )( pfCustomEventCbk pCbk, void * pUserData );

	/** \brief Sets the keepalive interval for SIP accounts
	 *
	 *  \ingroup user
	 *
	 *  The keepalive is a SIP packet sent over the signaling socket
	 *  containing only a new line (CRLF).  It is automatically enabled
	 *  for UDP sockets to keep alive any possible NAT mappings.  It has
	 *  the same effect as enabling STUN for this socket but unlike STUN
	 *  it will always keep the connection alive.
	 *
	 *  Set \p seconds to:
	 *  \li -1 to use the protocol's defaults
	 *  \li 0 to disable any keepalives
	 *  \li \> 0 to set the keepalive interval to that many seconds
	 *
	 *  Defaults (for -1) are 30 seconds for UDP and 180 seconds for TCP and TLS.
	 *  This setting will be applied to all subsequent SIP requests. For best effects
	 *  set this before registering.
	 *
	 *  \param[in] userId The user to configure
	 *  \param[in] seconds Keepalive setting (see above)
	 *
	 *  \sa AddUser(), RegisterUser(), AddStunServer()
	 */
	LIBRESULT (* SetUserKeepAliveTime )( UserHandler userId, int seconds );

	/** \brief Controls the user's presence watcherinfo support
	 *
	 *  \ingroup presence
	 *
	 *  The default is to subscribe for the user's presence watcherinfo
	 *  events when the user publishes its status with PublishStatus().
	 *
	 *  Use this to monitor who wants to subscribe for our presence in
	 *  server-based presence setups.  The server must support presence
	 *  watcher info and status publication.  Subscription authorization
	 *  is done on a sepparate channel (usually XCAP) that is not implemented
	 *  in this library.  This is not needed for peer-to-peer presence.
	 *
	 *  The library will generate WrapperCallbacks::onSubscriptionRequest()
	 *  events with \p SubscriberId set to INVALID_HANDLE for watchers in
	 *  pending/waiting state.  This means they cannot be authorized using
	 *  SubscriptionAccept() or rejected using SubscriptionReject().
	 *  They're meant to be authorized via XCAP or other means.
	 *
	 *  \param[in] UserId The user to configure
	 *  \param[in] enabled Set to 0 to disable, 1 to enable (default is enabled)
	 *
	 *  \sa PublishStatus(), WrapperCallbacks::onSubscriptionRequest()
	 */
	LIBRESULT (* SetUserPresenceWinfo )( UserHandler UserId, int enabled );

	/** \brief Configure KPML support for a user
	 *
	 *  \ingroup user
	 *
	 *  This will take effect in calls created/received after the setting
	 *  has changed. Current calls will not be affected.
	 *
	 *  \param[in] UserId The user to configure
	 *  \param[in] enabled Set to 1 to enable KPML
	 *
	 *  \sa SetUserDtmfBand()
	 */
	LIBRESULT (* SetUserKpmlEnabled )( UserHandler UserId, int enabled );

	/** \addtogroup crypto Cryptographic functions
	 *  @{
	 */

	/** \brief Decrypt data using RC4 and RSA-encrypted envelope.
	 *
	 *  This function is meant to decrypt data encoded using the PHP
	 *  envelope function. It creates a random RC4 key, encrypts the
	 *  data and then encrypts the RC4 with a RSA public key. The
	 *  encrypted RC4 key is the "envelope"; the RC4-encrypted data
	 *  is the content of the "envelope". This is meant to be used
	 *  live mostly (because of the comparative weakness of RC4 to RSA).
	 *
	 *  The pCipherData contains an optional EOL followed by base64
	 *  encoded RC4-encrypted content followed by an EOL followed by
	 *  base64 encoded RSA-encrypted envelope (the actual RC4 key)
	 *  followed by an EOL. The EOL could be either LF or CRLF.
	 *
	 *  \param[in] pCipherData Encrypted data
	 *  \param[out] pDecipherData Buffer for decrypted data (make it at least as big as pCipherData)
	 *  \param[in] DataLen Size of encrypted data in bytes
	 *  \param[in] pKey Buffer with private key (PEM)
	 *  \param[in] KeyLen Length of the pKey buffer
	 *  \param[in] pPassPhrase Password for the key, if encrypted
	 *  \param[out] pDataLen Resulting decrypted data length in bytes
	 */
	LIBRESULT (* RSADecryptData )( void * pCipherData, void * pDecipherData, int DataLen,
	                               void * pKey, int KeyLen, const char * pPassPhrase,
	                               int * pDataLen );

	/** \brief Generates a pseudo random key.
	 *
	 *  \param[out] pKeyBuffer Buffer for the key
	 *  \param[in] KeyLen Key size
	 */
	LIBRESULT (* GeneratePseudoRandomAESKey )( unsigned char * pKeyBuffer, AesKeyLength_t KeyLen );

	/** \brief Encrypts data using AES-CBC and then base64 encodes it
	 *
	 *  \param[in] pIn Input buffer (data to be encrypted)
	 *  \param[out] pOut Output buffer, should be twice as big as the input buffer. Result is zero terminated
	 *  \param[in] len Input buffer size in bytes
	 *  \param[in] key Key
	 *  \param[in] keylen Key size
	 */
	LIBRESULT (* AESEncryptDataInCBCMode )( const unsigned char * pIn, unsigned char  * pOut,
	                                        unsigned long len, const unsigned char * key,
	                                        AesKeyLength_t keylen );

	/** \brief Decrypts data using AES-CBC (it is first base64-decoded)
	 *
	 *  Unlike AESDecryptDataInCBCMode() this variant supports any base64 format.
	 *  It is recommended to use this instead of AESDecryptDataInCBCMode().
	 *
	 *  \param[in] pIn Input buffer (data to be decrypted)
	 *  \param[out] pOut Output buffer, should be as big as the input buffer
	 *  \param[in] len Input buffer size in bytes
	 *  \param[in] key Key
	 *  \param[in] keylen Key size
	 */
	LIBRESULT (* AESDecryptDataInCBCModePure )( unsigned char * pIn, unsigned char * pOut,
	                                            unsigned long len, const unsigned char * key,
	                                            AesKeyLength_t keylen );

	/** \brief Decrypts data using AES-CBC (base64 format is restricted)
	 *
	 *  Same as AESDecryptDataInCBCModePure() but the input base64 is restricted.
	 *  The input base64 data must be split into lines of 64 characters each
	 *  (what AESEncryptDataInCBCMode() and any other OpenSSL based application
	 *  produces). AESDecryptDataInCBCModePure() can handle both restricted and
	 *  unsplit base64 input.
	 *
	 *  \param[in] pIn Input buffer (data to be decrypted)
	 *  \param[out] pOut Output buffer, should be as big as the input buffer
	 *  \param[in] len Input buffer size in bytes
	 *  \param[in] key Key
	 *  \param[in] keylen Key size
	 */
	LIBRESULT (* AESDecryptDataInCBCMode )( unsigned char * pIn, unsigned char * pOut,
	                                        unsigned long len, const unsigned char * key,
	                                        AesKeyLength_t keylen );

	/** \brief Calculates a digest of a file
	 *
	 *  \param[in] fname Filename of the file to digest
	 *  \param[in] dt Digest algorithm
	 *  \param[out] out Buffer to receive binary digest (make it at least 64 bytes)
	 *  \param[out] out_len Size of the resulting digest in bytes
	 */
	LIBRESULT (* DigestFile )( const char *fname, DigestTypeEnum_t dt, unsigned char *out, int *out_len );

	/** \brief Calculates a digest from a memory buffer
	 *
	 *  \param[in] Data Buffer to digest
	 *  \param[in] DataLen Buffer size in bytes
	 *  \param[in] dt Digest algorithm
	 *  \param[out] out Buffer to receive binary digest (make it at least 64 bytes)
	 *  \param[out] out_len Size of the resulting digest in bytes
	 */
	LIBRESULT (* DigestData )( void * Data, int DataLen, DigestTypeEnum_t dt, unsigned char *out, int *out_len );

	/** \brief Encrypts data using a RSA private key
	 *
	 *  Encrypts a memory buffer using RSA private key. Size of the input
	 *  is limited by the key size.
	 *
	 *  \param[in] pData Buffer to encrypt
	 *  \param[in] DataLen Size in bytes of pData (limits depend on the key size, cannot exceed it!)
	 *  \param[in] pKeyPEM PEM encoded key to use
	 *  \param[in] KeyPEMLen Size in bytes of pKeyPEM in bytes
	 *  \param[in] pPassPhrase Pass phrase if the key is encrypted (nul terminated)
	 *  \param[out] pEncryptedData Buffer to receive encrypted data
	 *  \param[out] EncryptedDataLen in: size of the buffer, out: size of data after encryption (incl. optional b64)
	 *  \param[in] doBase64 Do base64 over the encrypted data. Provide twice as big buffer if you use this
	 */
	LIBRESULT (* RSAPrivateKeyEncrypt )( void * pData, int DataLen, void * pKeyPEM, int KeyPEMLen,
	                                     const char * pPassPhrase, void * pEncryptedData,
	                                     int * EncryptedDataLen, int doBase64 );

	/** @} */

	/** \brief Forces RFC 3264 for SIP calls
	 *
	 *  \ingroup user
	 *
	 *  Forces RFC 3264 media attributes for all calls made by this user.
	 *
	 *  \param[in] userId User for whom to force this option
	 *  \param[in] bForce Set to 1 to force, 0 to use automatic detection
	 */
	LIBRESULT (* SetUserForceRFC3264 )( UserHandler userId, int bForce );


	/** \addtogroup regex Regular Expression functions
	 *  @{
	 */

	/** \brief Adds and compiles a regular expression
	 *
	 * If the regular expression is a valid according to the basic or
	 * extended rules, a compiled structure will be created and a handle
	 * to it will be returned.
	 *
	 * Once the handle is available it can be used by any number of
	 * RegexMatch() requests.
	 *
	 * When the handle is no longer needed it can be destroyed with
	 * RemoveRegex().
	 *
	 * \param[in] pRegex UTF-8 regular expression
	 * \param[in] regexType Regular expression type (basic or extended)
	 * \param[in] pRegexId Resulting id (on success) that can be used with RegexMatch()
	 *
	 * \sa RegexMatch(), RemoveRegex()
	 */
	LIBRESULT (* AddRegex )( const char * pRegex, ScxRegexTypeEnum_t regexType, RegexHandler * pRegexId );

	/** \brief Execute a regular expression match
	 *
	 * Executes a regular expression match over a string. If a match is found the
	 * result will be L_OK otherwise it will be L_FAIL.
	 *
	 * If \p pBeg and \p pEnd are not NULL and there is a match they will receive
	 * the index of the starting byte of the match and the index of one byte past
	 * the end of the match.
	 *
	 * If there is more than one match \p pBeg and \p pEnd will point to the first
	 * match found.
	 *
	 * \param[in] RegexId The compiled regular expression id
	 * \param[in] pStr UTF-8 string to match
	 * \param[out] pBeg If a match is found, and pBeg is not NULL it will receive
	 *                  the position of the first byte of the match.
	 * \param[out] pEnd If a match is found, and pEnd is not NULL it will receive
	 *                  the position of the last byte of the match plus one.
	 */
	LIBRESULT (* RegexMatch )( RegexHandler RegexId, const char * pStr, int * pBeg, int * pEnd );

	/** \brief Remove a compiled regular expression
	 *
	 * Releases the handle and frees memory used by the compiled regular expression.
	 *
	 * \param[in] RegexId The regex Id to be removed
	 *
	 * \sa AddRegex(), RegexMatch()
	 */
	LIBRESULT (* RemoveRegex )( RegexHandler RegexId );

	/** @} */

	/** \brief Checks if a string is a SIP AoR or SIP Uri
	 *
	 *  \ingroup call
	 *
	 *  Processes the string as a dial target to check if it is a valid
	 *  SIP Uri or a valid SIP Address of Record or a tel uri.
	 *
	 *  SIP Uri: sip:user[:password]\@host[:port][;params]
	 *  SIP AoR: "Display Name" \<sip:user[:password]\@host[:port][;params]\>;[aor-params]
	 *  tel uri: tel:phonenumber[;params]
	 *
	 *  If the parse was successful, L_OK will be returned and the scheme and user pars
	 *  from the URI will be put into the \p pScheme and \p pUser variables (if not 0):
	 *  The actual scheme ("sip", "tel", "sips") is put into \p pScheme, limited by
	 *  \p schemeSize in octets.  The user part of the uri is put into \p pUser,
	 *  limited by \p userSize.
	 *
	 *  If pScheme is 0 or schemeSize \<= 0, the scheme will not be copied.
	 *  If pUser is 0 or userSize \<= 0, the user part will not be copied.
	 *
	 *  \param[in] pSipUri UTF-8 encoded zero-terminated string to parse
	 *  \param[out] pScheme UTF-8 encoded zero-terminated string containing the scheme of the uri
	 *  \param[in] schemeSize Limit in octets of the \p pScheme buffer
	 *  \param[out] pUser UTF-8 encoded zero-terminated string containing the user part from the uri
	 *  \param[in] userSize Limit in octets of the \p pUser buffer
	 */
	LIBRESULT (* ParseSipUri )( const char * pSipUri, char * pScheme, int schemeSize, char * pUser, int userSize );

	/** \brief Selects the SIP INFO DTMF behavior for a SIP user
	 *
	 *  \ingroup user
	 *
	 *  Only valid for SIP user accounts.  This function selects the way
	 *  SIP INFO DTMFs will be encoded.  If \p isSymbolic is 0, they will
	 *  be sent with their RFC 4733 (tel-event) number.  The duration
	 *  will be send as a number too.  If \p isSymbolic is 1, they will
	 *  be sent as symbols (0-9, *, #, A-D) and the duration will be sent
	 *  as a sequence of dashes (---------).  This behavior is required
	 *  by some vendors.
	 *  Only effective if the DTMF band is OUTBAND.
	 *  Default behavior is NOT symbolic (using tel-event numbers).
	 *
	 *  \param[in] userId The SIP user id to configure
	 *  \param[in] isSymbolic 0 for tel-event representation, 1 for symbolic representation
	 */
	LIBRESULT (* SetUserSipInfoSymbolic )( UserHandler userId, int isSymbolic );

	/** \brief Add a new contact and optionally subscribe for its presence/dialogs
	 *
	 *  \ingroup user
	 *
	 *  Only valid for SIP.  The full contact uri is generated from the user's
	 *  address (the contact must be on the same server). The subscriptions,
	 *  if successful, will be updated each \p subscribeSeconds seconds.
	 *
	 *  Updates about the contact will be delivered via callbacks.
	 *
	 *  Values for the \p subscribeFlags parameter:
	 *  \li CONTACT_SUBSCRIBE_PRESENCE, subscribe for presense
	 *  \li CONTACT_SUBSCRIBE_BLF, subscribe for call tracking (BLF)
	 *
	 *  Pre-defined bit sets for \p subscribeFlags:
	 *  \li CONTACT_SUBSCRIBE_NONE, don't subscribe (chat still available)
	 *  \li CONTACT_SUBSCRIBE_ALL, subscribe for everything
	 *
	 *  \param[in] UserId The user to make the subscription
	 *  \param[in] pContactNumber Username/number of the contact to subscribe for
	 *  \param[out] pContact The resulting contact id
	 *  \param[in] subscribeSeconds The expire time of the subscription
	 *  \param[in] refreshSeconds Reserved, set to -1
	 *  \param[in] subscribeFlags Which services to subscribe for
	 *
	 *  \sa RemoveContact(), RefreshContact(), SendPlainMessage(),
	 *      WrapperCallbacks::onContactStatus(),
	 *      WrapperCallbacks::onContactTerminated(),
	 *      WrapperCallbacks::onContactRetrying()
	 */
	LIBRESULT (* AddContact2 )( UserHandler UserId, const char * pContactNumber, ContactHandler * pContact, int subscribeSeconds, int refreshSeconds, int subscribeFlags );

	/** \brief Adds a timed custom event to the callback queue
	 *
	 *  \ingroup event
	 *
	 *  This will make the function pointed by \p pCbk to be called with
	 *  its parameter set to \p pUserData after \p delayMs milliseconds.
	 *  The function will be called from PollEvents() so there could be
	 *  additional delay depending on the polling interval.
	 *
	 *  Use this function to schedule your own events to be executed in
	 *  your event handling thread (from the same thread or a different
	 *  one).
	 *
	 *  \param[in] pCbk Pointer to the function to call
	 *  \param[in] pUserData The parameter of the function
	 *  \param[in] delayMs Delay in milliseconds
	 *
	 *  \sa PollEvents(), AddCustomEvent()
	 */
	LIBRESULT (* AddTimedCustomEvent )( pfCustomEventCbk pCbk, void * pUserData, long delayMs );

	/** \brief Set a SIP user to impersonate a Cisco device
	 *
	 *  \ingroup user
	 *
	 *  Must be set before registering.  This will add some Cisco-specific
	 *  parameters to the registration.
	 *
	 *  \param[in] userId The user
	 *  \param[in] deviceType Device type id
	 *  \param[in] pDeviceName Device name as registered in CUCM
	 *
	 *  \sa RegisterUser()
	 */
	LIBRESULT (* SetUserCiscoDevice )( UserHandler userId, eCiscoDeviceType_t deviceType, const char * pDeviceName );

	/** \brief Select timing source (affects Windows only)
	 *
	 *  \ingroup audio
	 *
	 *  Due to many problems related to QueryPerformanceCounter's accuracy
	 *  (on Windows systems) it can be replaced with timeGetTime() using
	 *  this function.
	 *
	 *  IT MUST BE CALLED BEFORE InitCallManager()
	 *
	 *  It is a no-op for all other platforms.
	 *
	 *  \param[in] TGTEnabled 0 for QPC (default), 1 for TGT
	 */
	LIBRESULT (* UseTimeGetTime )( int TGTEnabled );

	/** \brief Reserved
	 *  \ingroup reserved
	 */
	LIBRESULT (* Reserved001 )();
	
	/** \brief Reserved
	 *  \ingroup reserved
	 */
	LIBRESULT (* Reserved002 )();
	
	/** \brief Reserved
	 *  \ingroup reserved
	 */
	LIBRESULT (* Reserved003 )();
	
	/** \brief Reserved
	 *  \ingroup reserved
	 */
	LIBRESULT (* Reserved004 )();
	
	/** \brief Reserved
	 *  \ingroup reserved
	 */
	LIBRESULT (* Reserved005 )();


	/** \brief Enable SIP Header dumps for SIP Calls
	 *
	 *  \ingroup user
	 *
	 *  Enable or disable the SIP header fields dump for SIP calls for the specified user.
	 *
	 *  \param[in] UserId The User to configure (must be SIP)
	 *  \param[in] enable 0 to disable, 1 to enable
	 *
	 *  \sa WrapperCallbacks::onSipCallHeaderFields(),
	 *      WrapperContext::SipUserAddHeader(),
	 *      WrapperContext::SipUserClearHeader()
	 */
	LIBRESULT (* SipUserHeaderDump )( UserHandler UserId, int enable );

	/** \brief Add a SIP Header field for SIP calls
	 *
	 *  \ingroup user
	 *
	 *  Adds a header field with its value to the list of custom headers to
	 *  add to each SIP call made by this SIP user.
	 *
	 *  This could break the SIP request to the point that we will not even
	 *  attempt to send it. Use with great care.
	 *
	 *  This function will automatically reject to touch the following header fields:
	 *  Via, Contact, From, To, CallID, CSeq
	 *
	 *  Use WrapperContext::SipUserClearHeader() to clear all values for a
	 *  given header field or all custom header fields added by this function.
	 *
	 *  \param[in] UserId The user to configure (must be SIP)
	 *  \param[in] pHeaderFieldName The header field name as a UTF-8 nul-terminated string
	 *  \param[in] pHeaderFieldValue The header field value as a UTF-8 nul-terminated string
	 *
	 *  \sa WrapperContext::SipUserClearHeader(),
	 *      WrapperContext::SipUserHeaderDump(),
	 *      WrapperCallbacks::onSipCallHeaderFields()
	 */
	LIBRESULT (* SipUserAddHeader )( UserHandler UserId, const char * pHeaderFieldName, const char * pHeaderFieldValue );

	/** \brief Clear (all) custom SIP Header fields
	 *
	 *  \ingroup user
	 *
	 *  Clears all header fields that match \p pHeaderFieldName unless it is NULL in
	 *  which case all custom header fields added by WrapperContext::SipUserAddHeader()
	 *  for the same user will be cleared.
	 *
	 *  \param[in] UserId User to configure
	 *  \param[in] pHeaderFieldName Heade field name to match or NULL for all
	 *
	 *  \sa WrapperContext::SipUserAddHeader(),
	 *      WrapperContext::SipUserHeaderDump(),
	 *      WrapperCallbacks::onSipCallHeaderFields()
	 */
	LIBRESULT (* SipUserClearHeader )( UserHandler UserId, const char * pHeaderFieldName );

	/** \brief Returns the current SIP TCP port
	 *
	 *  \ingroup init
	 *
	 *  Returns the currently open local TCP port for SIP TCP. It might not be
	 *  seen as the same number from outside in case of NAT.
	 *
	 *  \param[out] TcpPort The local TCP port for SIP TCP
	 *  \sa WrapperContext::GetSipPort(), WrapperContext::SetSipTcpPort()
	 */
	LIBRESULT (* GetSipTcpPort )( int * TcpPort );

	/** \brief Returns the current SIP TLs port
	 *
	 *  \ingroup init
	 *
	 *  Returns the currently open local TCP port for SIP TLS (not a mistake,
	 *  TLS run on top of TCP). It might not be seen as the same number from
	 *  outside in case of NAT.
	 *
	 *  \param[out] TlsPort The local TCP port for SIP TLS
	 *  \sa WrapperContext::GetSipPort(), WrapperContext::SetSipTlsPort()
	 */
	LIBRESULT (* GetSipTlsPort )( int * TlsPort );

	/** \brief Returns the current IAX2 UDP port
	 *
	 *  \ingroup init
	 *
	 *  Returns the currently open local UDP port for IAX2. It might not be
	 *  seen as the same number from outside in case of NAT.
	 *
	 *  \param[out] IaxPort The local UDP port for IAX2
	 *  \sa WrapperContext::GetSipPort()
	 */
	LIBRESULT (* GetIaxPort )( int * IaxPort );

	/** \brief Initialize TLS transport for SIP
	 *
	 *  \ingroup init
	 *
	 *  Initializes the TLS transport for SIP along with the User Agent
	 *  Server part (incoming TLS connections need a domain and cert to
	 *  work properly).
	 *
	 *  Although it is not recommended to call this function more than once
	 *  between InitCallManager() and DestroyCallManager(), it is not forbidden.
	 *
	 *  Calling this function a second time without destroying the call manager
	 *  will replace any domain certificates but will not reset the TLS transport
	 *  objects (that is not supported on a very low level in reSIProcate). It
	 *  might result in new TLS transport objects being created behind th secene.
	 *
	 *  To initialize TLS with minimum configuration use:
	 *
	 *  SipInitTls( \<CIPHERS\>, \<SUITE\>, 0, 0, 0 );
	 *
	 *  This will initialize only the default transport; incoming TLS connections
	 *  will not work unless a user certificate is later configured.
	 *
	 *  To simulate previous behaviour use:
	 *
	 *  SipInitTls( \<CIPHERS\>, \<SUITE\>, "", 0, 0 );
	 *
	 *  This will try to guess the local hostname and will generate a self-signed
	 *  certificate for a TLS transport that will listen for incoming TLS
	 *  connections. This will not stop the default TLS transport creation nor
	 *  will it force any users to use it for their outgoing TLS connections.
	 *
	 *  Using self-signed domain TLS certificate is rarely supported by TLS peers
	 *  and in most cases will not work. It is recommended not to use it.
	 *
	 *  To load a domain certificate and configure the domain name from it,
	 *  instead of guessing:
	 *
	 *  SipInitTls( \<CIPHERS\>, \<SUITE\>, 0, \<PATH\>, [\<PASSPHRASE\>] )
	 *
	 *  To load a domain certificate but try to guess the local domain name, use:
	 *
	 *  SipInitTls( \<CIPHERS\>, \<SUITE\>, "", \<PATH\>, [\<PASSPHRASE\>] )
	 *
	 *  To load a domain certificate and override the domain name, use:
	 *
	 *  SipInitTls( \<CIPHERS\>, \<SUITE\>, \<DOMAIN\>, \<PATH\>, [\<PASSPHRASE\>] )
	 *
	 *  Detailed information for the parameters:
	 *
	 *  \param[in] onlyStrongCiphers If 1, will limit the ciphers to 3DES and AES
	 *             (RC4 or DES will not be allowed)
	 *  \param[in] secureSuite Select SSLv2/v3 or TLSv1 to use (was always
	 *             set to TLSv1 before)
	 *  \param[in] domain If 0, and if \p domainCert is 0, disable the domain
	 *             transport.
	 *             If 0, but \p domainCert is not 0, use the domain name from the
	 *             cert.
	 *             If "" (empty string), try discovering the domain
	 *             automatically. Has a low chance of succeeding. Falls back to
	 *             "localhost". Does not matter what is the value of \p domainCert
	 *             If not 0, and not empty (not ""), will use this domain, no
	 *             matter what is the value of \p domainCert
	 *  \param[in] domainCert If 0, and \p domain is 0, disable the domain
	 *             transport.
	 *             If 0, and \p domain is not 0, create a self-signed
	 *             certificate to use for the incoming connections. Not
	 *             recommended.
	 *             If not 0, try loading a Certificate and Key pair from the
	 *             file with this name. The file can be in PEM format (the
	 *             order in which the certificate and key are pasted in it
	 *             does not matter) or in PKCS#12 format (.PFX, the way the
	 *             pair is exported in Windows). If it is in the PKCS#12
	 *             format any additional certificates will be added to the
	 *             trusted certificate list. In this case \p domains how the
	 *             transport's domain name is configured.
	 *  \param[in] domainCertPassphrase If \p domainCert is not 0 this parameter
	 *             can optionally contain the plaintext passphrase protecting the
	 *             key in \p domainCert . Can be 0 if the key is not protected.
	 *
	 *  \sa VerifyUserCertificate(), SetUserCertificate()
	 */
	LIBRESULT (* SipInitTls )( int onlyStrongCiphers, eSecureSuite_t secureSuite, const char * domain, const char * domainCert, const char * domainCertPassphrase );

	/** \brief Load or generate a User certificate (obsolete)
	 *
	 *  \ingroup user
	 *
	 *  This function is obsolete and here for backward compatibility.
	 *
	 *  Use SetUserTlsConfig() for finer control of SIP TLS users.
	 *
	 *  The previous version of this function did not allow for SIP TLS users
	 *  to create outgoing TLS connections without a certificate. This issue
	 *  is rectified in this version and the default "minimum" recommended
	 *  parameters now do exactly this.
	 *
	 *  Internally, this function calls SetUserTlsConfig(). The following
	 *  describes how exactly it is being called behind the scene.
	 *
	 *  If \p filename is 0, the SIP user object will be configured to use
	 *  the default TLS transport and create outgoing connections without a
	 *  user certificate. Uses the E_SCUSR_CLIENT_ONLY setting.
	 *
	 *  If \p filename is the empty string (not 0, but ""), the user will be
	 *  configured to use the global TLS server domain, if available. If such
	 *  is not available the user will not use a certificate. Uses the
	 *  E_SCUSR_COMMON setting.
	 *
	 *  If \p filename is not 0 and is not the empty string ("") the user will
	 *  be configured to use a dedicated TLS server transport. \p filename
	 *  points to a file from which to load the certificate for the transport.
	 *  It can be either a combined certificate+key PEM file or a PKCS#12
	 *  (.PFX for Windows) file. Additional certificates in the latter case
	 *  are added as trusted root certificates. Uses the E_SCUSR_DEDICATED
	 *  setting.
	 *
	 *  \param[in] UserId The User to configure
	 *  \param[in] filename Certificate file name, see above
	 *  \param[in] passPhrase If not 0, try decrypting the user key with this
	 *             passphrase when loading from \p filename .
	 *
	 *  \sa SetUserTlsConfig(), SipInitTls(), VerifyUserCertificate()
	 */
	LIBRESULT (* SetUserCertificate )( UserHandler UserId, const char * filename, const char * passPhrase );

	/** \brief Disables all certificate verification
	 *
	 *  \ingroup init
	 *
	 *  Disables all certificate verification.
	 *
	 *  Currently affects only SIP with TLS.
	 *
	 *  This is a very dangerous setting and it is highly recommended to be
	 *  reset on phone startup (i.e. don't store it in the configuration files).
	 *
	 *  \param[in] disable If 1, disable verification.
	 *                     If 0, enable verification.
	 */
	LIBRESULT (* DisableCertVerification )( int disable );

	/** \brief Set automatic microphone selection and configuration
	 *
	 *  \ingroup volume
	 *
	 *  Sets the automatic microphone input source selection and
	 *  configuration.
	 *
	 *  The automatic selection does the following:
	 *  - Find the first suitable microphone attached to the selected
	 *    input device. On Windows this is done via the component
	 *    type ID. On the other platform the literal "microphone"
	 *    is searched for (when the names are not in english this
	 *    does not succeed).
	 *  - If a suitable microphone source is found, it is unmuted and
	 *    the audio device is configured to use it as input.
	 *
	 *  When the automatic microphone selection is turned off, the
	 *  currently selected source will be used.
	 *
	 *  The input sources are configured when the first audio request
	 *  is made (including sometimes just a sound playback). This
	 *  function can be called any time before this happens to take
	 *  effect. After a new input device is selected it will be
	 *  consulted again.
	 *
	 *  The automatic selection does not remember the original state
	 *  of the system mixer. It will not restore the old mixer status
	 *  if turned off. Once a change has been made it will remain until
	 *  the user has manually changed it from the mixer or via this
	 *  API (currently no API calls available to select the sources XXX)
	 *
	 *  \param[in] enable If 1, enable automatic microphone selection [default]
	 *                    If 0, disable automatic microphone selection
	 *
	 */
	LIBRESULT (* SetAutoMicSelection )( int enable );


	/** \brief Set playback gain
	 *
	 *  \ingroup sound
	 *
	 *  Sets the playback gain for the playback-related calls. Does not
	 *  apply to sound playback (for that use the normal volume controls).
	 *
	 *  Default is 100 (no change).
	 *
	 *  If set to 0, it will effectively mute the playback.
	 *
	 *  Can be set to values larger than 100 to amplify the signal. There
	 *  is clipping check for this case.
	 *
	 *  \param[in] gainPerCent The gain in % ( can be \> 100 )
	 */
	LIBRESULT (* SetPlaybackGain )( int gainPerCent );

	/** \brief Get playback gain
	 *
	 *  \ingroup sound
	 *
	 *  Gets the current playback gain.
	 *
	 *  \param[out] pGainPerCent Gets the current playback gain
	 */
	LIBRESULT (* GetPlaybackGain )( int * pGainPerCent );

	/** \brief Pause/resume a sound during playback
	 *
	 *  \ingroup sound
	 *
	 *  Toggles pause/resume on a sound that is currently being played.
	 *
	 *  Note that to pause/resume the sound used in the Playback* API
	 *  you should use PausePlayback(). Due to internal limitations, when
	 *  a sound is used for playback it is being copied. The sound
	 *  handler for the copied object is not known to this API user.
	 *  PausePlayback() also takes care of pausing/resuming the monitoring
	 *  sound (which is the original sound passed to StartPlayback().
	 *
	 *  \param[in] soundId The sound handler
	 *  \param[in] pause 1 to pause, 0 to resume
	 */
	LIBRESULT (* PauseSound )( SoundHandler soundId, int pause );

	/** \brief Pause/unpause the playback
	 *
	 *  \ingroup sound
	 *
	 *  Toggles pause/resume on the current playback and its monitoring
	 *  sound, if any.
	 *
	 *  \param[in] pause 1 to pause, 0 to resume
	 */
	LIBRESULT (* PausePlayback )( int pause );

	/** \brief Start audio sample dumps
	 *
	 *  \ingroup audio
	 *
	 *  Enables audio sample dumps from the VoiceProcessor:
	 *    - vpldump.wav (8 and 16 kHz dump; lower 16 kHz from 32 kHz dump)
	 *    - vphdump.wav (only in 32 kHz mode, the higher 16 kHz)
	 *
	 *  There are no dumps if all three voice processing filters are disabled
	 *  (no AEC, no AGC and no NS).
	 *
	 *  The dumps are reset every time the VoiceProcessor is reset. This
	 *  happens at least in the following situations:
	 *    - when a call is started when there were no calls at all
	 *    - when a call is started on a higher frequency than currently
	 *      existing calls
	 *    - when a call is retrieved from hold when no other calls were
	 *      active
	 *    - the input or output devices are changed
	 *    - other similar situations that require VoiceProcessor
	 *      reconfiguration.
	 */
	LIBRESULT (* StartAudioDebugDump )( void );

	/** \brief Start audio sample dumps
	 *
	 *  \ingroup audio
	 *
	 *  Stops the audio dump started by StartAudioDebugDump()
	 */
	LIBRESULT (* StopAudioDebugDump )( void );

	/** \brief Enable or disable detailed error reporting
	 *
	 *  \ingroup global
	 *
	 *  When detailed errors are enabled the cause codes returned
	 *  in the callbacks listed below will change into a unique
	 *  error code.  To get the old style Q.931 code use the GetCauseCode()
	 *  function.  To access the detailed error information use the
	 *  GetDetailedError() and FreeDetailedError() functions.
	 *  Even if GetDetailedError() is not used, FreeDetailedError()
	 *  must be called to free the detail structure.
	 *
	 *  WrapperCallbacks::onUserRegistrationFailure()
	 *  WrapperCallbacks::onCallRejected()
	 *  WrapperCallbacks::onCallHangup()
	 *  WrapperCallbacks::onCallTransferFailure()
	 *  WrapperCallbacks::onCallFailure()
	 *  WrapperCallbacks::onFaxError()
	 *  WrapperCallbacks::onMessageFailed()
	 *
	 *  \param[in] enable 1 to enable, 0 to disable detailed errors
	 *  \sa WrapperContext::GetCauseCode(), WrapperContext::GetDetailedError(),
	 *      WrapperContext::FreeDetailedError(), WrapperCallbacks::onUserRegistrationFailure(),
	 *      WrapperCallbacks::onCallRejected(), WrapperCallbacks::onCallHangup(),
	 *      WrapperCallbacks::onCallTransferFailure(), WrapperCallbacks::onCallFailure(),
	 *      WrapperCallbacks::onFaxError(), WrapperCallbacks::onMessageFailed()
	 */
	LIBRESULT (* EnableDetailedErrors )( int enable );

	/** \brief Returns the old style Q.931 code from a new style error code
	 *
	 *  \ingroup global
	 *
	 *  Extracts the old style Q.931 code from a new style unique error
	 *  code.  Initially the error is encoded in the lower 8 bits but
	 *  this can change so it's best to use this function instead of
	 *  doing newCode & 0xff.
	 *
	 *  \param[in] newCode New style code
	 *  \return Old style Q.931 code
	 *  \sa WrapperContext::EnableDetailedErrors(), WrapperContext::GetDetailedError(),
	 *      WrapperContext::FreeDetailedError()
	 */
	int (* GetCauseCode )( int newCode );

	/** \brief Get detailed error information
	 *
	 *  \ingroup global
	 *
	 *  Returns detailed information about an error code returned from one
	 *  of these callbacks:
	 *
	 *  WrapperCallbacks::onUserRegistrationFailure()
	 *  WrapperCallbacks::onCallRejected()
	 *  WrapperCallbacks::onCallHangup()
	 *  WrapperCallbacks::onCallTransferFailure()
	 *  WrapperCallbacks::onCallFailure()
	 *  WrapperCallbacks::onFaxError()
	 *  WrapperCallbacks::onMessageFailed()
	 *
	 *  Detailed error reporting must be enabled via EnableDetailedErrors().
	 *
	 *  The next error code, if not zero, points to additional information
	 *  for the same error condition. Currently not used.
	 *
	 *  In case of E_LAYER_SIP or E_LAYER_SIP_LOCAL, layerCode is a
	 *  SIP response status code. For E_LAYER_IAX and E_LAYER_IAX_LOCAL
	 *  layerCode is a Q.931 cause code (same as q931code). For the
	 *  rest of the layers the code is an internal error code currently
	 *  not exported.
	 *
	 *  E_LAYER_WRAPPER:    Error condition in the wrapper layer. Check
	 *                      the protocol to see if it's in the SIP or IAX
	 *                      managers.
	 *  E_LAYER_SIP:        Error message from the network
	 *  E_LAYER_SIP_LOCAL:  Error condition in the SIP layer resulting in a
	 *                      locally generated SIP error message.
	 *  E_LAYER_IAX:        Error message from the network
	 *  E_LAYER_IAX_LOCAL:  Error condition in the IAX layer
	 *  E_LAYER_XMPP:       Error message from the network
	 *  E_LAYER_XMPP_LOCAL: Error condition in the XMPP layer
	 *
	 *  \param[in] errorCode The error code returned by one of the callbacks
	 *  \param[out] q931code Old style Q.931 code
	 *  \param[out] proto Protocol (PROTO_SIP, PROTO_IAX)
	 *  \param[out] layer Layer where this error originated from
	 *  \param[out] layerCode Layer-specific error code
	 *  \param[out] errorStr Error string, null terminated UTF-8
	 *  \param[out] sourceFileName File name of the source file where the
	 *              error originated
	 *  \param[out] sourceFileLine Line number in the source file
	 *  \param[out] functionName Name of the function in the source file
	 *  \param[out] nextErrorCode Next error code in the stack
	 *  \sa WrapperContext::EnableDetailedErrors(), WrapperContext::GetCauseCode(),
	 *      WrapperContext::FreeDetailedError(), WrapperCallbacks::onUserRegistrationFailure(),
	 *      WrapperCallbacks::onCallRejected(), WrapperCallbacks::onCallHangup(),
	 *      WrapperCallbacks::onCallTransferFailure(), WrapperCallbacks::onCallFailure(),
	 *      WrapperCallbacks::onFaxError(), WrapperCallbacks::onMessageFailed()
	 */
	LIBRESULT (* GetDetailedError )( int errorCode, int * q931code, ProtoType_t * proto,
	                                 eErrorLayer_t * layer, int * layerCode, const char ** errorStr,
	                                 const char ** sourceFileName, int * sourceFileLine,
	                                 const char ** functionName, int * nextErrorCode );

	/** \brief Get context information about the error
	 *
	 *  \ingroup global
	 *
	 *  Returns the object class and handler for which the error occured.
	 *
	 *  \param[in] errorCode The new style error code
	 *  \param[out] objClass The object class
	 *  \param[out] handle The handle of the object
	 */
	LIBRESULT (* GetDetailedErrorContext )( int errorCode, eObjectClass_t * objClass, Handler * handle );

	/** \brief Free detailed error information
	 *
	 *  \ingroup global
	 *
	 *  Frees detailed information about one or more errors.
	 *  If errorCode is zero, all errors for the handle will be freed.
	 *  If errorCode is non zero, only that error will be freed.
	 *
	 *  Use FreeDetailedError( 0, INVALID_HANDLE ) to free
	 *  all codes for all objects.
	 *
	 *  \param[in] errorCode Error code to free, or 0 to use handle
	 *  \param[in] handle Handle to specify which errors to free,
	 *             if errorCode is zero, otherwise ignored.
	 */
	LIBRESULT (* FreeDetailedError )( int errorCode, Handler handle );

	/** \brief Add a detailed error
	 *
	 *  \ingroup global
	 *
	 *  Adds a new detailed error.  The new error's code is returned
	 *  in errorCodeOut.  The error can be retrieved by using
	 *  GetDetailedError() and GetDetailedErrorContext().
	 *
	 *  \param[out] errorCodeOut The extended error code
	 *  \param[in] q931code Q.931 (ISDN) code (required)
	 *  \param[in] proto Prototype (or PROTO_UNKNOWN)
	 *  \param[in] layer Layer (or E_LAYER_UNKNOWN)
	 *  \param[in] layerCode Layer-specific error code (or 0)
	 *  \param[in] errorStr Error string text in UTF-8 (should be set but can be 0)
	 *  \param[in] objClass Object class (or E_OBJ_UNKNOWN)
	 *  \param[in] handler Handler of the object (or INVALID_HANDLE)
	 *  \param[in] sourceFileName Source file name, UTF-8 (or 0)
	 *  \param[in] sourceFileLine Source file line (or 0)
	 *  \param[in] functionName Function name (or 0)
	 *  \param[in] nextErrorCode Error code returned by this function
	 *                           if you want to chain several errors.
	 */
	LIBRESULT (* AddDetailedError )( int * errorCodeOut, int q931code, ProtoType_t proto,
	                                 eErrorLayer_t layer, int layerCode, const char * errorStr,
	                                 eObjectClass_t objClass, Handler handler,
	                                 const char * sourceFileName, int sourceFileLine,
	                                 const char * functionName, int nextErrorCode );

	/** \brief Creates a call manager instance for a protocol
	 *
	 *  \ingroup global
	 *
	 *  Creates a call manager instance for a specific protocol.
	 *  Will be automatically destroyed by DestroyCallManager().
	 *  Some call managers are automatically created by
	 *  InitCallManager(). SIP cannot be created by this function.
	 *
	 *  \param[in] proto Protocol
	 *  \param[in] port Port at which to bind the main socket
	 *
	 *  \returns L_OK If manager was created or already running
	 *  \returns L_INVALIDARG If the manager was not running but
	 *                        cannot be created by this function.
	 */
	LIBRESULT (* StartCallManager )( ProtoType_t proto, int port );

	/** \brief Check if a call manager is running for a protocol
	 *
	 *  \ingroup global
	 *
	 *  Checks if a call manager is running for a specific protocol.
	 *  Works for call managers started automatically by InitCallManager()
	 *  and for call managers started explicitly by StartCallManager().
	 * 
	 *  \param[in] proto Protocol
	 *  \param[out] port If the protocol is running, the port at which
	 *                   the main socket is bound
	 *
	 *  \returns L_OK If the manager is running.
	 */
	LIBRESULT (* IsCallManagerRunning )( ProtoType_t proto, int * port );
	
	/** \brief Controls the system-provided in echo canceller (AEC)
	 *
	 *  \ingroup audio
	 *
	 *  Enables or disables the built in Echo Cancellation engine. Default is
	 *  disabled.
	 *  Current support: iOS only.
	 *  Has no effect on systems that don't have a built in AEC.
	 *
	 *  \param[in] enabled 0 to disable, 1 to enable
	 */
	LIBRESULT (* UseSystemEchoCancellation )( int enabled );
	
	/** \brief Controls the system-provided Automatic Gain Control (AGC)
	 *
	 *  \ingroup audio
	 *
	 *  Enables or disables the built in AGC. Default is disabled.
	 *  Current support: iOS only.
	 *  Has no effect on systems that don't have a built in AGC.
	 *
	 *  \param[in] enabled 0 to disable, 1 to enable
	 */
	LIBRESULT (* UseSystemAGC )( int enabled );
	
	/** \brief Controls the system-provided non-voice audio ducking
	 *
	 *  \ingroup audio
	 *
	 *  Enables or disables the built in non-voice audio ducking.
	 *  Default is disabled.
	 *  Current support: iOS only.
	 *  Has no effect on systems that don't support ducking.
	 *
	 *  \param[in] enabled 0 to disable, 1 to enable
	 */
	LIBRESULT (* UseSystemAudioDucking )( int enabled );

	/** \brief Probes for available transports for the given SIP settings
	 *
	 *  \ingroup global
	 *
	 *  Probes consecutively for transport availability over SIP with
	 *  the given account settings.
	 *
	 *  The tests are done via SIP REGISTER requests with the given settings.
	 *  No actual SIP user will be used for this. Instead a separate
	 *  temporary profile will be used. No user-specific callbacks will
	 *  be fired for this process.
	 *
	 *  During the TLS checks a Certificate-related callback can be fired
	 *  which will usually result in TLS being rejected as a viable transport
	 *  and the probing will continue with TCP.
	 *
	 *  If the TLS settings are changed the probing can be restarted.
	 *
	 *  The profile generated for the TLS test will benefit from any
	 *  certificates that were added for a SIP user with the same
	 *  configuration.
	 *
	 *  STUN and rport will not be used or tested. This may cause the
	 *  probing to fail because of NAT issues.
	 *
	 *  The transports are checked in this order:
	 *
	 *  1. TLS
	 *  2. TCP
	 *  3. UDP
	 *
	 *  Each test will generate an informative callback. The process
	 *  will be considered successful after the first successful REGISTER.
	 *  The newly created binding on the server will be removed (the
	 *  temporary profile will be unregistered).
	 *
	 *  \param[in] domain Domain (SIP user creation: realm entry)
	 *  \param[in] proxy Outbound proxy (SIP user creation: server entry)
	 *  \param[in] username Username
	 *  \param[in] authUsername Authentication username
	 *  \param[in] password Authentication password
	 *  \param[out] pProbeId Probe handle for callbacks
	 *
	 *  \returns L_OK the probing has started, \p pProbeId contains the
	 *                handle for the callbacks
	 *  \returns L_FAIL the probing can't be started (mainly, InitCallManager()
	 *                  was not called or other general error), \p pProbeId
	 *                  is not touched, no callbacks can arrive
	 *
	 *  \sa WrapperContext::AddUser(), WrapperCallbacks::onProbeError(),
	 *      WrapperCallbacks::onProbeState(), WrapperCallbacks::onProbeFailed(),
	 *      WrapperCallbacks::onProbeSuccess()
	 */
	LIBRESULT (* ProbeSipTransport )( const char * domain, const char * proxy,
		const char * username, const char * authUsername, const char * password,
		ProbeHandler * pProbeId );

	/** \brief Controls the Automatic Gain Control filter
	 *
	 *  \ingroup audio
	 *
	 *  Enables or disables the AGC filter.
	 *  Works on all platforms. For Desktop platforms the AGC includes
	 *  both analog and digital adaptive control by controlling
	 *  the analog gain for the microphone device. For mobile
	 *  platforms only an adaptive digital gain is applied to
	 *  the audio coming from the microphone.
	 *
	 *  The GUI is advised to bar the user from manipulating the
	 *  hardware gain of the microphone. The GUI can also poll
	 *  the audio input level via GetAudioInputLevel() once or
	 *  twice a second.
	 *
	 *  \param[in] AGCEnabled 0 to disable, 1 to enable
	 */
	LIBRESULT (* UseAutomaticGainControl )( int AGCEnabled );

	/** \brief Controls the Noise Suppression filter
	 *
	 *  \ingroup audio
	 *
	 *  Enables or disables the Noise Suppression filter.
	 *  Works on all platforms.
	 *
	 *  \param[in] NSEnabled 0 to disable, 1 to enable
	 */
	LIBRESULT (* UseNoiseSuppression )( int NSEnabled );

	/** \brief Enables shorter codecs negotiation if possible
	 *
	 *  \ingroup user
	 *
	 *  Enables short form codec negotiation for protocols using
	 *  SDP (currently SIP only).
	 *  This is disabled by default, i.e. codes are negotiated in
	 *  full. Full negotiation can be problematic for SIP over UDP
	 *  on networks with low MTU such as VPNs or even on normal
	 *  networks when using video.
	 *
	 *  \param[in] UserId User for which to set short codec negotiation
	 *  \param[in] Enable 0 to disable, 1 to enable short codec negotiation
	 */
	LIBRESULT (* SetUserShortCodecNegotiation )( UserHandler UserId, int Enable );

	/** \brief Resets the video encoder
	 *
	 *  \ingroup video
	 *
	 *  Destroys and re-creates the video encoder, video encoder
	 *  thread, video encoding frame buffer, etc.
	 *
	 *  If the new configuration (width,height,fps,bps) is the same
	 *  as the old one this call can be a no-op.
	 *
	 *  \param[in] pThreadId Thread handle for the video stream
	 *  \param[in] width New width of the video image
	 *  \param[in] height New height of the video image
	 *  \param[in] fps New frame rate of the video stream
	 *  \param[in] bps New bit rate in bits per second for the encoder
	 *
	 *  \sa VideoSendFrame()
	 */
	LIBRESULT (* VideoResetEncoder )( void * pThreadId, int width, int height, float fps, int bps );

	/** \brief Verifies usability for SSL certificate and key pair
	 *
	 *  \ingroup global
	 *
	 *  This function can be used to check if a certificate and key pair
	 *  found in a PEM or PKCS#12 file is valid before applying it to
	 *  a user.
	 *
	 *  This gives much more detail than SetUserCertificate which still
	 *  MUST be called to actually configure the user.
	 *
	 *  The code tries to isolate the most common errors like trying to
	 *  load an encrypted private key with the incorrect password or
	 *  trying to use a wrong combination of key and certificate.
	 *
	 *  Actual certificate signature validation is not done yet although
	 *  we have decided to reserve an output parameter for it.
	 *
	 *  \param[in]  suite The suite for which we want to test the cert
	 *  \param[in]  pFileName File name of the cert+key pair. Accepts
	 *              PEM (text file with the cert and key one after the
	 *              other in base64 encoding) or PKCS#12 (a binary format
	 *              more common on Windows)
	 *  \param[in]  pPassphrase Optional, the pass phrase which is used to
	 *              protect the private key in the file
	 *  \param[out] pResult Result from the check
	 *  \param[out] pVerifyError Reserved for future use
	 */
	LIBRESULT (* VerifyUserCertificate)( eSecureSuite_t suite, const char * pFileName,
	                                     const char * pPassphrase, eSecureCertResult_t * pResult,
	                                     eSecureCertError_t * pVerifyError );

	/** \brief Configure TLS for a user
	 *
	 *  \ingroup user
	 *
	 *  This function replaces SetUserCertificate().
	 *
	 *  The new name is due to the need to not only configure certificates
	 *  for TLS but also to run without one or to assign the user to a
	 *  global TLS transport.
	 *
	 *  Although this function is meant to be used for different protocols,
	 *  currently it only supports SIP.
	 *
	 *  A new enum, eSecureUserConfig_t, is used to select the behavior for
	 *  the user object.
	 *
	 *  E_SCUSR_CLIENT_ONLY
	 *
	 *    This setting switches the user to a certificate-less operation,
	 *    which was not available in previous versions of the WRAPPER.
	 *    This mode is used by the majority of TLS client applciations and
	 *    should be the default setting.
	 *    This mode ignores \p fileName and \p passPhrase .
	 *    For SIP, rport is almost certainly required for this to work properly
	 *    before we add support for RFC 5626 (SIP Outbound). This will try
	 *    to force any requests over TLS to come back over our outbound TLS
	 *    connection as we won't have a working TLS server for this user.
	 *
	 *  E_SCUSR_COMMON
	 *
	 *    In case WRAPPER was configured to accept incoming TLS connections,
	 *    there is a global TLS server object which can be shared between
	 *    the users of the same protocol (SIP only for now).
	 *    This mode ignores \p fileName and \p passPhrase .
	 *    For SIP, rport is highly recommended but not required for this mode
	 *    because we can receive incoming TLS connections unless a firewall
	 *    interferes with our traffic.
	 *
	 *  E_SCUSR_DEDICATED
	 *
	 *    This mode is available in case the TLS server requires user
	 *    certificates. The user certificate must be provided in \p fileName
	 *    and if the key along with the certificate is encrypted, the
	 *    encryption passphrase must be provided in \p passPhrase .
	 *    The format for the key+cert combination is either PEM or PKCS#12.
	 *    Additional certificates in case the format is PKCS#12 are added to
	 *    the trusted root certificate authorities list.
	 *    For SIP, rport is highly recommended. See E_SCSRV_COMMON's notes on
	 *    rport above.
	 *
	 *  E_SCUSR_GENERATE
	 *
	 *    This mode behaves like the previous version of SetUserCertificate().
	 *    It will generate a self-signed certificate with the protocol-level
	 *    URI as certificate subject (this is the SIP URI for SIP users). It
	 *    will then create a dedicated TLS server transport and bind it to the
	 *    generated certificate.
	 *    This mode ignores \p fileName and \p passPhrase .
	 *    Not recommended.
	 *
	 *  \param[in] UserId The User to configure
	 *  \param[in] usrConf The TLS server mode setting (see above)
	 *  \param[in] fileName Used to point to a PEM or PKCS#12 certificate to
	 *             load for this user, depending on \p usrConf .
	 *  \param[in] passPhrase Used to provide the encryption pass phrase in
	 *             case a certificate is being loaded via \p fileName .
	 *
	 *  \sa SetUserCertificate(), SipInitTls(), VerifyUserCertificate()
	 */
	LIBRESULT (* SetUserTlsConfig )( UserHandler UserId, eSecureUserConfig_t usrConf, const char * fileName, const char * passPhrase );

	/** Adds a certificate to the exception list
	 *
	 *  \ingroup global
	 *
	 *  Adds the given certificate to the list of exceptions.
	 *
	 *  This can be useful if the user wants to force a SIP/TLS connection
	 *  with a server that presents a broken certificate.
	 *
	 *  The PEM data and its length can be taken straight from the
	 *  onSecureCertError() failure callback.
	 *
	 *  The next attempt to communicate with a server using the same
	 *  certificate will succeed. For SIP registrations over TLS this
	 *  will happen automatically. Other usages (for example calls) will
	 *  have to be left for the user to retry.
	 *
	 *  The user should be warned that using exceptions makes TLS much
	 *  less secure than they think it is.
	 *
	 *  \param[in] data This points to a buffer containing a certificate in
	 *             PEM format
	 *  \param[in] dataLen The length of the data buffer in octets
	 *
	 *  \sa AddCertificatesDirect(), WrapperCallbacks::onSecureCertError()
	 */
	LIBRESULT (* AddKnownCertificateDirect )( const char * pCertDataPEM, int certDataLen );

	/** \brief Set the default mix channel for a user
	 *
	 *  \ingroup user
	 *
	 *  Changes the default mix channel for calls made with this user,
	 *  both outgoing and incoming.
	 *
	 *  \param[in] UserId User ID
	 *  \param[in] MixType Mix channel type
	 *
	 *  \sa SetCallMixType()
	 */
	LIBRESULT (* SetUserMixType )( UserHandler UserId, eStreamMixType_t MixType );

	/** \brief Change the mix channel for a call
	 *
	 *  \ingroup call
	 *
	 *  Changes the mix channel for a running call. The effect is immediate.
	 *
	 *  \param[in] CallId The call
	 *  \param[in] MixType The new mix type
	 *
	 *  \sa SetUserMixType
	 */
	LIBRESULT (* SetCallMixType )( CallHandler CallId, eStreamMixType_t MixType );

	/** \brief Set session timers for a SIP user
	 *
	 *  \ingroup user
	 *
	 *  Changes the session timers setting according to RFC 4028.
	 *
	 *  SIP Only (for now)
	 *
	 *  The RFC describes two general modes when session timers are not
	 *  disabled: UAC and UAS.
	 *
	 *  UAC means that the one who makes the call (the client or caller)
	 *  will try to refresh the session periodically to make sure it is
	 *  still alive.
	 *
	 *  UAS means that the one who receives the call (the server or
	 *  callee) will try to refresh the session.
	 *
	 *  We have two more settings, "local" and "remote". When "local" is
	 *  selected, we'll try to be the ones to do the refreshes. This means
	 *  that we will use the "UAC" setting for outgoing calls and will
	 *  prefer the "UAS" setting for incoming calls.
	 *
	 *  The "remote" mode will do the opposite of the "local" mode. It will
	 *  try to force the other end of the call to do the refreshes.
	 *
	 *  The final decision is always at the one who provides the answer
	 *  which does not always means this will be the callee (especially
	 *  when the SIP call was done using the INVITE-no-offer mode).
	 *
	 *  If the session timers are enabled we will always have a periodic
	 *  refresh attempts, no matter if the remote end supports this feature.
	 *  This is according to RFC 4028.
	 *
	 *  If a re-INVITE (a refresh) fails the call is considered broken and
	 *  will be closed with an error. The error can be modified by any
	 *  proxies in between.
	 *
	 *  \param[in] UserId The user to configure
	 *  \param[in] TimerMode The mode to use
	 *  \param[in] ExpirySec Session expiration in seconds, if TimerMode
	 *             is not E_TIMER_DISABLED. Must be \>= 90 (seconds).
	 *
	 *  \returns L_INVALIDARG If ExpirySec is too short, L_FAIL if the SIP
	 *           user is not found or if this is not a SIP user or if the
	 *           SipCallManager has failed to initialize.
	 */
	LIBRESULT (* SetUserSessionTimers )( UserHandler UserId, eSessionTimerMode_t TimerMode, int ExpirySec );

	/** \brief Reserved
	 *  \ingroup reserved
	 */
	LIBRESULT (* Reserved006 )();
	
	/** \brief Reserved
	 *  \ingroup reserved
	 */

	/** \brief Stop the activation process
	 *
	 *  \ingroup global
	 *
	 *  Cancels any activation in progress.
	 */
	LIBRESULT (* StopActivation )( void );



	/** \brief Configure external audio
	 *
	 *  \ingroup audio
	 *
	 *  Only for external audio builds.
	 *
	 *  Asks the library to configure itself according to these parameters.
	 *  The actual parameters are obtained from the external audio API
	 *  which is out of this library's scope.
	 *
	 *  \param[in] sampleRateHz Sampling rate
	 *  \param[in] samplesPerFrame Recommended samples per frame
	 *  \returns L_OK if the parameters were accepted
	 */
	LIBRESULT (* ExternalAudioInit )( int sampleRateHz, int samplesPerFrame );

	/** \brief Process some samples
	 *
	 *  \ingroup audio
	 *
	 *  Only for external audio builds.
	 *
	 *  Asks the library to process some audio samples. The samplesIn array
	 *  must contain samplesCount samples coming from the microphone. These
	 *  samples will be resampled if needed, then encoded and sent over the
	 *  network. The samplesOut array will receive samplesCount samples
	 *  from the library's mixer. The mixed sounds include streams coming
	 *  from the network, sounds played by request of the library user etc.
	 *
	 *  The library will handle all mixing, encoding, decoding, resampling
	 *  and other processing as needed.
	 *
	 *  At least one of the buffers must not be NULL.
	 *
	 *  latencyMs specifies the latency in milliseconds as follows:
	 *  If both samplesIn and samplesOut are not NULL, latencyMs specifies
	 *  the combined latency for the input and output.
	 *  If only samplesIn is set, latencyMs specifies the microphone latency.
	 *  If only samplesOut is set, latencyMs specifies the speaker latency.
	 *
	 *  \param[in] samplesIn Contains some samples coming from the microphone
	 *  \param[out] samplesOut Will receive some samples from the library's mixer
	 *  \param[in] samplesCount Number of samples in the samplesIn and samplesOut buffers
	 *  \param[in] latencyMs Latency in milliseconds (used for the echo cancellation).
	 *  \returns L_OK to continue processing or L_FAIL to stop the audio
	 */
	LIBRESULT (* ExternalAudioFrame )( const short * samplesIn, short * samplesOut, int samplesCount, int latencyMs );

	/** \addtogroup zrtp ZRTP management functions
	 *  @{
	 */

	/** \brief Configure global ZRTP ZID Cache file
	 *
	 *  Sets the full file name for the global ZRTP ZID Cache file. The file
	 *  is in a file format similar to CSV. It is managed entirely by the
	 *  WRAPPER based on RFC 6189 (ZRTP).
	 *
	 *  If the file name is an empty string or NULL pointer, the ZRTP will
	 *  proceed in cacheless mode.
	 *
	 *  The file name is UTF-8 encoded and will be converted to the native
	 *  Unicode encoding for the system. (Yes, this means no need for special
	 *  handling on Windows!)
	 *
	 *  The cache file is used to store binary keys called "retained secrets"
	 *  in the ZRTP protocol. These retained secrets are obtained from a
	 *  successful ZRTP negotiation with a peer.
	 *
	 *  Each device or phone capable of ZRTP has its own ZRTP ID called "ZID".
	 *  When doing the ZRTP handshake this ZID is exchanged and can later be
	 *  used to associate information with a ZRTP peer.
	 *
	 *  The ZRTP negotiation includes confirmation that the person on the other
	 *  end is who they claim they are. It is always recommended to confirm
	 *  the person by their voice. The ZRTP cache is used to also confirm that
	 *  the person is using a device that we've already interacted with.
	 *
	 *  In the very first call with a peer we will not have a cache entry.
	 *  After that we expect our cache to match the peer's cache. In case this
	 *  does not happen we might have a security problem. This means confirming
	 *  the negotiation with the short authentication string.
	 *
	 *  Just in case, here are some details about the file format:
	 *
	 *  The first line is our own ZID, base64-encoded. The ZID is a unique
	 *  ZRTP identifier consisting of 96 random bits. The WRAPPER will generate
	 *  a new ZID for new cache files. Once generated, our ZID will not change
	 *  unless a new cache file is configured or it is lost.
	 *
	 *  From the second line to the end of the file we have peer cache records,
	 *  single record per line with fields separated by the pipe symbol '|'.
	 *
	 *  Each peer record consists of:
	 *    - The peer's ZID, base64 encoded
	 *    - Retained Secret 1, base64 encoded. The retained secret is a one-way
	 *      256-bit hash produced from a previous successful ZRTP negotiation.
	 *      It will be used to confirm the peer identity for subsequent ZRTP
	 *      negotiations without the need for SAS confirmation.
	 *    - Retained Secret 1 expiration, ISO format, or +INF if the retained
	 *      secret will never expire
	 *    - Retained Secret 2, base64 encoded. After every successful ZRTP
	 *      negotiation involving a Diffie-Hellman key exchange, a new retained
	 *      secret is obtained. What was previously the "Retained Secret 2"
	 *      is deleted. What was previously the "Retained Secret 1" becomes the
	 *      new "Retained Secret 2". The brand new retained secret is stored
	 *      as the new "Retained Secret 1".
	 *    - Retained Secret 2 expiration, ISO format, or +INF for "never expires"
	 *      or -INF for "not available yet".
	 *
	 * \param[in] pCacheFileName The ZID Cache file name, or 0 to disable
	 *
	 */
	LIBRESULT (* SetGlobalZrtpCache )( const char * pCacheFileName );

	/** \brief Add a ZRTP Hash Algorithm for a User
	 *
	 *  Adds the specified ZRTP Hash Algorithm to the bottom of the
	 *  User's ZRTP Hash Algorithms list.
	 *
	 *  Note that there is limited support for hash algorithms.
	 *
	 *  Note that the ZRTP RFC requires S256 to be always present. This means that
	 *  when doing the actual ZRTP negotiation the library might offer S256 as the
	 *  lowest prirority even if S256 was not added with this function.
	 *
	 *  The hash algorithm is used in various steps in the ZRTP negotiation.
	 *  Bigger number of bits in the hash mean higher securty and slower computation.
	 *  It is recommended to add all supported algorithms, putting the most
	 *  secure ones first.
	 *
	 *  \param[in] UserId The user ID
	 *  \param[in] HashAlgorithm The hash algorithm
	 *
	 *  \sa ClearUserZrtpHashList()
	 */
	LIBRESULT (* AddUserZrtpHash )( UserHandler UserId, eZRTPHashAlgorithm_t HashAlgorithm );

	/** \brief Clears the list of ZRTP Hash Algorithms for a User
	 *
	 *  Clears the list of ZRTP Hash Algorithms for a User.
	 *
	 *  \param[in] UserId The user ID
	 *
	 *  \sa AddUserZrtpHash()
	 */
	LIBRESULT (* ClearUserZrtpHashList )( UserHandler UserId );

	/** \brief Add a ZRTP Cipher Algorithm for a User
	 *
	 *  Adds the specified ZRTP Cipher Algorithm to the bottom of the user's
	 *  ZRTP Cipher Algorithms list.
	 *
	 *  Note that there might be limited support for cipher algorithms.
	 *
	 *  Note also that AES1 is required by the standard and even if not present
	 *  in a user's cipher list, it will be offered.
	 *
	 *  The cipher algorithm is the actual symmetric cipher used to encrypt the
	 *  audio data once the ZRTP negotiation has completed successfully. It is
	 *  mainly used by the SRTP protocol for this purpose (ZRTP is NOT the one
	 *  doing the actual audio encryption; SRTP is still used for this).
	 *
	 *  The original SRTP RFC describes only AES1 from the list of ciphers in the
	 *  ZRTP RFC. Most peers will only support AES1. Please confirm which ciphers
	 *  are supported by the library.
	 *
	 *  Higher numbers behind the cipher name means better encryption and slower
	 *  processing.
	 *
	 *  \param[in] UserId The user ID
	 *  \param[in] CipherAlgorithm The cipher algorithm
	 *
	 *  \sa ClearUserZrtpCipherList()
	 */
	LIBRESULT (* AddUserZrtpCipher )( UserHandler UserId, eZRTPCipherAlgorithm_t CipherAlgorithm );

	/** \brief Clears the list of ZRTP Cipher Alogrithms for a User
	 *
	 *  Clears the list of ZRTP Cipher Algorithms for a User.
	 *
	 *  \param[in] UserId The user ID
	 *
	 *  \sa AddUserZrtpCipher()
	 */
	LIBRESULT (* ClearUserZrtpCipherList )( UserHandler UserId );

	/** \brief Add a ZRTP Authentication Tag type for a User
	 *
	 *  Add a ZRTP Authentication Tag type for a User.
	 *
	 *  Note that there might be limited support for authentication tag types.
	 *
	 *  Note that HS32 and HS80 are requred by the standard and even if not present
	 *  in a user's auth tag type list they will be negotiated. If both are not
	 *  present, HS80 will have higher priority than HS32.
	 *
	 *  The authentication tag is used to authenticate each encrypted audio frame
	 *  send over the SRTP channel once the ZRTP negotiation has completed.
	 *
	 *  The original SRTP specification describes HS32 and HS80 which are 32-bit
	 *  and 80-bit HMAC-SHA1 authentication tags. The 80-bit tag provides better
	 *  security.
	 *
	 *  The Skein-MAC authentication tags were not in the original SRTP RFC and
	 *  might not be supported by the peer or even this library. Please confirm
	 *  this before using it.
	 *
	 *  \param[in] UserId The user ID
	 *  \param[in] AuthTag The authentication tag type
	 *
	 *  \sa ClearUserZrtpAuthList()
	 */
	LIBRESULT (* AddUserZrtpAuth )( UserHandler UserId, eZRTPAuthTag_t AuthTag );

	/** \brief Clears the list for ZRTP Authentication Tags for a User
	 *
	 *  Clears the list for ZRTP Authentication Tags for a User.
	 *
	 *  \param[in] UserId The user ID
	 *
	 *  \sa AddUserZrtpAuth()
	 */
	LIBRESULT (* ClearUserZrtpAuthList )( UserHandler UserId );

	/** \brief Add a ZRTP Key Agreement algorithm for a User
	 *
	 *  Add a ZRTP Key Agreement algorithm for a User.
	 *
	 *  Note that there might be limited support for key agreement algorithms.
	 *
	 *  Note that DH3K is required by the standard and even if not present in this
	 *  list it will be negotiated.
	 *
	 *  The key agreement algorithm is the main feature of the ZRTP process. There
	 *  are three types described in the RFC.
	 *
	 *  1. Finite Field Diffie-Hellman. This is the standard public key exchange
	 *     used in many security protocols. It involves complex calculations with
	 *     huge integer numbers. The ZRTP protocol uses two public prime groups
	 *     for the key agreement: the 3072-bit group and the 2048-bit group. The
	 *     names 'DH3k' and 'DH2k' come from the prime group size. The 3027-bit
	 *     group provides better security (obviosly).
	 *
	 *  2. Elliptic Curve Diffie-Hellman. This is a newer public key exchange also
	 *     employed in many security protocols (most of the newer versions of the
	 *     same protocols that use FFDH). ECDH uses smaller numbers, but they're
	 *     still huge and the calculations are still expensive. The two supported
	 *     curve groups by the ZRTP standard are the 384-bit and 256-bit groups.
	 *     The names 'EC25' and 'EC38' come from the group size. EC38 provides
	 *     better security.
	 *
	 *  3. Preshared. This key exchange does not rely on any public key exchange
	 *     algorithm but instead relies on both parties sharing a common secret
	 *     which is configured on each peer before the ZRTP negotiation is made.
	 *     This negotiation type is used when a peer lacks the CPU or memory
	 *     necessary for a full Diffie-Hellman exchange.
	 *
	 *  EC38 and DH3k are recommended for their better security. EC38 is preferred
	 *  because it is less CPU-intensive than DH3k and provides similar security.
	 *
	 *  Please confirm that PRSH (Preshared) is available in the Library before
	 *  using it.
	 *
	 *  \param[in] UserId The user ID
	 *  \param[in] KeyAgreement The key agreement algorithm
	 *
	 *  \sa ClearUserZrtpKeyAgreementList()
	 */
	LIBRESULT (* AddUserZrtpKeyAgreement )( UserHandler UserId, eZRTPKeyAgreement_t KeyAgreement );

	/** \brief Clears the list of ZRTP Key Agreement algorithms for a User
	 *
	 *  Clears the list of ZRTP Key Agreement algorithms for a User.
	 *
	 *  \param[in] UserId The user ID
	 *
	 *  \sa AddUserZrtpKeyAgreement()
	 */
	LIBRESULT (* ClearUserZrtpKeyAgreementList )( UserHandler UserId );

	/** \brief Add a ZRTP SAS encoding for a User
	 *
	 *  Add a ZRTP SAS encoding for a User.
	 *
	 *  The SAS stands for "Short Authentication String". It is used to confirm
	 *  the public key exchange and to do "biometric" voice authentication.
	 *
	 *  The public key exchange employed by ZRTP is vulnerable to man-in-the-middle
	 *  attacks. By doing vocal comparison of the resulting key exchange the two peers
	 *  both establish each other's voice authenticity (assuming they know each other
	 *  and that the VoIP codec is not awful) and make sure the result is the same.
	 *
	 *  If an attacker tries to hijack the ZRTP key exchnage the two parties will
	 *  end up with different results. By requiring voice authentication the attacker
	 *  now has to fake the voices of both peers (and do it not only for the key
	 *  exchange but for the entire call too).
	 *
	 *  This voice authentication does not really work for peers that can't recognize
	 *  each other's voices. In case a secure call is desired with a unknown party,
	 *  a preshared secret must be employed.
	 *
	 *  The SAS is used to make comparing the result easier. Instead of reading a
	 *  long binary sequence ZRTP employs a simple conversion of the binary code
	 *  into a short easy to read text. Only the most significant bits from the
	 *  result are used.
	 *
	 *  B32 (Base-32) converts the 20 most significant bits into 4 alphanumeric
	 *  characters which are chosen from the latin alphabet combined with some
	 *  digits so there are no ambiguous characters in it (like 1, l and I for
	 *  example). To compare the SAS the two participants must know how to spell
	 *  the latin letters and the digits from 0 to 9 (with some omissions).
	 *
	 *  B256 (Base-256) converts the 32 most significant bits into 4 english words
	 *  from a carefully selected dictionary produced by a complicated genetic
	 *  algorithm. This SAS type requires the two user to be able to read and pronounce
	 *  correctly each of the 512 words in the dictionary. They don't need to know
	 *  the words by heart by they do need to be able to pronounce them unambiguously.
	 *  This might not be suitable for non-english speakers.
	 *
	 *  Note that B32 is always required to be present in the negotiation and
	 *  the library will add it to the negotiation even if it is not present in
	 *  the list.
	 *
	 *  \param[in] UserId The User ID
	 *  \param[in] SasEncoding The SAS encoding
	 *
	 *  \sa ClearUserZrtpSasEncodingList()
	 */
	LIBRESULT (* AddUserZrtpSasEncoding )( UserHandler UserId, eZRTPSASEncoding_t SasEncoding );

	/** \brief Clears the list of SAS encodings for a User
	 *
	 *  Clears the list of SAS encodings for a User.
	 *
	 *  \param[in] UserId The user ID
	 *
	 *  \sa AddUserZrtpSasEncoding()
	 */
	LIBRESULT (* ClearUserZrtpSasEncodingList )( UserHandler UserId );

	/** \brief Enable or disable ZRTP for a User
	 *
	 *  Enable or disable ZRTP for a User. The ZRTP negotiation happens regardless of
	 *  other security features. It does not require processing in the signaling layer
	 *  (SIP/SDP) and it does not require the signaling layer to be encrypted (works
	 *  for SIP/UDP and SIP/TCP.
	 *
	 *  ZRTP negotiation can also happen for users that are using SIP/TLS and have
	 *  SRTP SDES enabled. This does not make much sense but is technically possible.
	 *
	 *  Enabling ZRTP does not mean that the call will fail if the ZRTP negotiation
	 *  fails. The call will still proceed, although the library controller might very
	 *  well decide to drop the call if there is a ZRTP error.
	 *
	 *  \param[in] UserId The user ID
	 *  \param[in] Enabled 1 to enable ZRTP, 0 to disable ZRTP
	 */
	LIBRESULT (* SetUserZrtpEnabled )( UserHandler UserId, int Enabled );

	/** \brief Set ZRTP cache expiry for a User
	 *
	 *  After a ZRTP negotiation between two peers is complete they update their
	 *  caches based on their configuration and the negotiation expiry times.
	 *
	 *  The cache is only useful if both peers in a negotiation have one. In
	 *  case one of the peers does not have a cache there is absolutely no point
	 *  in caching for the other peer.
	 *
	 *  In case both peers have working caches they need to negotiate the lifetime
	 *  of the cached data. For this to happen both send to the other end their
	 *  preferences for the cache expiry. After both receive each other's cache
	 *  expiry preferences, they choose the smaller of the two.
	 *
	 *  The expiry is set in seconds from the time of the negotiation. There are
	 *  two special values: 0 disables caching for this exchange and -1 sets the
	 *  expiry to infinity (never expire). -1 is the default (also recommended by
	 *  the protocol).
	 *
	 *  When comparing expiration, -1 is treated as the maximum value. This means
	 *  that if one peer is configured to never expire, and the other is configured
	 *  to expire in 3 days, both will set the expiry to 3 days.
	 *
	 *  If either one of the peers set the expiry to 0 both peers will delete
	 *  their cache entries for each other.
	 *
	 *  \param[in] UserId The user ID
	 *  \param[in] ExpirySeconds The expiry in seconds or 0 for no cache or
	 *                           -1 for infinity (default)
	 */
	LIBRESULT (* SetUserZrtpCacheExpiry )( UserHandler UserId, int ExpirySeconds );

	/** \brief Enable or disable ZRTP for a Call
	 *
	 *  Enable or disable ZRTP for a Call. Even if a user does not have ZRTP enabled
	 *  it is possible to try to negotiated ZRTP for any call made by the user (so long
	 *  as it uses RTP, which means SIP).
	 *
	 *  It is not guaranteed that the remote end will reply to the ZRTP negotiation
	 *  request (in which case an error will be reported but the call will not be dropped).
	 *
	 *  It is possible to retry ZRTP in the same call that has failed to negotiate ZRTP
	 *  already.
	 *
	 *  Disabling ZRTP for a call means to stop encrypting and "go clear". This is a part
	 *  of the ZRTP standard and is optional. It might not be supported by the remote
	 *  end or the request might be rejected.
	 *
	 *  \param[in] CallId The call ID
	 *  \param[in] Enabled 1 to enable ZRTP, 0 to disable ZRTP
	 */
	LIBRESULT (* CallEnableZrtp )( CallHandler CallId, int Enabled );

	/** \brief Confirm or reject the SAS for a ZRTP call
	 *
	 *  Once ZRTP negotiation is done, the call switches to encrypted audio using
	 *  the negotiated keys.
	 *
	 *  Depending on the situation a SAS comparison might be needed to confirm the
	 *  authenticity of the participants. Even if the cache confirms the peer a SAS
	 *  comparison is not a bad idea.
	 *
	 *  Use this call to indicate whether the comparison was successful. In case of
	 *  failure the call will not be dropped.
	 *
	 *  If the negotiation is confirmed (by setting \p Confirmed to 1), the peer will
	 *  be added to our cache (in case we have a cache). The next call with this
	 *  peer will benefit from the added security of the cache.
	 *
	 *  If the negotation is rejected (by setting \p Confirmed to 0), the peer will
	 *  be removed from the cache (in case we have a cache). For the next call the
	 *  peer will be treated as a stranger and the SAS confirmation will be requsted.
	 *
	 *  Rejecting a peer can be done regardles off the KnwonPeer and CacheMismatch
	 *  parameters of WrapperCallbacks::onCallZrtpSuccess(). Rejecting a peer will
	 *  always lead to removing them from our cache.
	 *
	 *  \param[in] CallId The call ID
	 *  \param[in] Confirmed 1 if the SAS is confirmed, 0 if the SAS is incorrect
	 */
	LIBRESULT (* CallConfirmZrtpSas )( CallHandler CallId, int Confirmed );

	/** @} */

	/** \brief Enables or disables the fixed gain on the audio output 
	 *
	 *  \ingroup audio
	 *
	 *  Enable or disable the fixed speaker gain confgured by
	 *  SetFixedSpeakerGain(). By default, the gain is not applied.
	 *
	 *  The filter that applies the gain is using a fixed point
	 *  calculation (multiplication + shift + saturation checks).
	 *  It should be very fast but you should not enable the gain
	 *  unless you have set it to something different than 0.0.
	 *
	 *  The effect will be immediate. You can call this function
	 *  at any time after InitCallManager(). This means you can
	 *  enable/disable the gain during a call.
	 *
	 *  \param[in] GainEnabled 1 to enable, 0 to disable gain
	 *
	 *  \sa SetFixedSpeakerGain()
	 **/
	LIBRESULT (* UseFixedSpeakerGain )( int GainEnabled );

	/** \brief Configures the fixed gain on the audio output 
	 *
	 *  \ingroup audio
	 *
	 *  Configures the gain to be applied when the gain filter is
	 *  enabled by UseFixedSpeakerGain().
	 *
	 *  The gain is expressed in decibels (dB). It can be anything
	 *  but some values will produce very distorted and loud noise,
	 *  while others will effectively mute the audio.
	 *
	 *  Here are some example values, but please do tests and choose
	 *  what best suits your case. The precision of the fixed-point
	 *  calculation is 0.001.
	 *
	 *  +20.0 dB: Huge audio boost; almost certain to produce
	 *            loud noise unless the input is very quiet.
	 *            Don't use values above this.
	 *   +6.0 dB: Moderate boost
	 *   +2.0 dB: Small boost
	 *    0.0 dB: No boost
	 *   -6.0 dB: Moderate decrease in volume. Better use the hardware
	 *            volume controls.
	 *   -100 dB: Almost always will produce complete silence.
	 *
	 *  If you are familiar with how the audio is processed, here
	 *  is the filter formula:
	 *
	 *  gain(s) = s * 10 ^ ( db / 20.0 )
	 *
	 *  Here gain is the gain filter; s is the sample being processed;
	 *  db is the gain value in dB. We're using the amplitude forumla
	 *  because of the way PCM is representing the audio.
	 *
	 *  As with UseFixedSpeakerGain(), you can call this function at
	 *  any time after InitCallManager().
	 *
	 *  \param[in] dB Gain in dB
	 *
	 *  \sa UseFixedSpeakerGain()
	 **/
	LIBRESULT (* SetFixedSpeakerGain )( double dB );

	/** \brief Configures the WAV music file for the Music on Hold stream
	 *
	 *  \ingroup global
	 *
	 *  Loads a new music file to be used in the music stream. If there are
	 *  any calls using the music stream they will immediately switch to the
	 *  new music stream.
	 *
	 *  To get detailed error use LoadMusicServiceFile2().
	 *
	 *  Current format support:
	 *
	 *  WAV file, PCM format with the following limitations:
	 *    - The format tag must be 1 (PCM) or 0xFFFE (PCM EXTENSIBLE)
	 *    - The bits per sample can be 8, 16, 24 or 32. 16-bit sample size
	 *      is the native format. The other sample formats will be
	 *      automatically resampled to 16 bit.
	 *    - The sampling rate must be something sane. Depending on the call
	 *      codec it might get up- or down-sampled on the fly. Most common
	 *      sampling rates are supported (8, 16, 32, 44.1, 48 kHz).
	 *      If the WAV is 44.1kHz and there are two calls, one narrowband
	 *      at 8 kHz and one fullband at 48 kHz, the music server will
	 *      produce a downsampled 8 kHz sound to the narrowband call and a
	 *      upsampled (44.1 kHz -> 48 kHz) sound to the fullband call.
	 *    - The channel count can be anything. Mono is the native format.
	 *      Stereo and above will be automatically downmixed to mono.
	 *
	 *  \param[in] fileNameUtf8 UTF-8 file name of the music file
	 *
	 *  \returns L_OK if the file is loaded. L_FAIL if there is an error or
	 *           if the file format is not supported.
	 *
	 *  \sa SetUserMusicService(), SetCallMusicService(), LoadMusicServiceFile2()
	 *
	 **/
	LIBRESULT (* LoadMusicServiceFile )( const char * fileNameUtf8 );

	/** \brief Configures Music on Hold for a User or Globally
	 *
	 *  \ingroup user
	 *
	 *  Configures MOH for a user object. All calls created from this
	 *  user object will have this setting.
	 *
	 *  If UserId is set to INVALID_HANDLE this will configure MOH
	 *  for incoming calls that have no matching user. This will also
	 *  change the defaults for all users created afterwards and their
	 *  calls. Users that have already been created will not be affected.
	 *
	 *  \param[in] UserId The user to configure
	 *  \param[in] Enabled 1 to enable, 0 to disable MOH
	 *
	 *  \sa LoadMusicServiceFile(), SetCallMusicService()
	 */
	LIBRESULT (* SetUserMusicService )( UserHandler UserId, int Enabled );

	/** \brief Configures Music on Hold for an active Call
	 *
	 *  \ingroup call
	 *
	 *  Configures MOH for an active call. This does not put the call
	 *  on hold and does not retrieve a call from hold. This function only
	 *  marks the call to use MOH the next time CallHold() is invoked.
	 *
	 *  In case this function is called after CallHold() has been called,
	 *  but before CallUnhold() has been called, it will not interfere
	 *  with the current hold process. This means the call will stay on
	 *  whatever hold method is being use, be it music service or normal
	 *  protocol hold.
	 *
	 *  This function affects only subsequent CallHold() usages.
	 *
	 *  \param[in] CallId The call to configure
	 *  \param[in] Enabled 1 to enable, 0 to disable MOH
	 *
	 *  \sa LoadMusicServiceFile(), SetUserMusicService()
	 */
	LIBRESULT (* SetCallMusicService )( CallHandler CallId, int Enabled );

	/** \brief Configures the audio driver
	 *
	 *  \ingroup audio
	 *
	 *  Originally intended for use on Android only.
	 *
	 *  This will allow switching between the currently available audio
	 *  providers. Although all modes are available on all platforms, currently
	 *  only the Android GUI has the ExternalAudio API implemented. This might
	 *  be useful for SDK developers.
	 *
	 *  The default approach by the WRAPPER is to either use Portaudio or if
	 *  no suitable hardware is available to use the Virtual driver. Use the
	 *  \p nDriverType parameter to force a specific driver provider or to
	 *  revert to the default. Use E_AUDIO_DRV_NO_CHANGE to leave the provider
	 *  unchanged.
	 *
	 *  For most platforms the audio driver will be able to also select the
	 *  best native sampling rate to be used. The \p nSampleRate parameter can
	 *  be used to force a specific native sampling rate.
	 *  Use E_AUDIO_DRV_RATE_NO_CHANGE to leave the rate unchanged.
	 *  Use a specific sampling ratein Hz (any positive number) outside of the
	 *  pre-defined values of the eAudioDriverRate_t enum to select a specific
	 *  sampling rate.
	 *
	 *  The WRAPPER can fine-tune the native buffer sizes in case a portaudio
	 *  driver is being used. To force a specific native buffer size use the
	 *  \p nBufferSizeInFrames parameter. Not all portaudio host APIs will
	 *  acknowledge this setting. The size is given in frames. To leave the
	 *  buffer setting unchanged, use the E_AUDIO_DRV_BUFFER_NO_CHANGE pre-defined
	 *  value. To revert to the default use the E_AUDIO_DRV_BUFFER_DEFAULT pre-defined
	 *  value.
	 *
	 *  \param[in] nDriverType The provider to be used by the driver
	 *  \param[in] nSampleRate The sampling rate or a special value
	 *  \param[in] nBufferSizeInFrames The buffer size in frames or a special value
	 */
	LIBRESULT (* SetAudioDriverConfiguration )( eAudioDriverEngine_t nDriverType, int nSampleRate,  int nBufferSizeInFrames );
	
	/** \brief Sets the ringing device volume,
	 *
	 * \ingroup audio
	 *
	 *  Sets the ringing device volume, between 0.0 (muted) and 1.0 (maximum volume).
	 *  In many cases the ringing device and the output device are the same, so chaning
	 *  one level will also change the other.
	 *
	 *  \param[in] level The audio level between 0.0 and 1.0
	 *
	 *  \sa SetAudioInputLevel(), SetAudioOutputLevel(), GetAudioRingLevel()
	 */
	LIBRESULT (* SetAudioRingLevel )( double level );

	/** \brief Gets the current ringing device volume
	 *
	 * \ingroup audio
	 *
	 *  Returns the volume setting for the ringing device.  Values are between
	 *  0.0 and 1.0.  This is not the measured energy of the ringing samples.
	 *
	 *  In many cases the output device and the ringing device are actually the
	 *  same device. This means both volume controls will report the same volume.
	 *
	 *  \return The volume setting for the ringing device between 0.0 and 1.0
	 *
	 *  \sa SetAudioRingLevel(), GetAudioOutputLevel()
	 */
	double (* GetAudioRingLevel )( void );
	
	/** \brief Sets the resampler used by audiodriver
	 *
	 *  Selects the resampler to be used by the audio engine. In most cases
	 *  the native sampling rate of the audio hardware will not match the sampling
	 *  rate of the active VoIP call. In this case a resampler is employed to
	 *  do sample rate conversion. Different methods produce different quality
	 *  and use algorithms of different complexity resulting in different CPU
	 *  usage. In some cases the resampler cannot be selected.
	 *
	 *  A special type, E_AUDIO_DRV_DEFAULT lets the audio driver select the
	 *  reampler. This is the default setting.
	 *
	 *  \param[in] nResamplerType The resampler type to use
	 * 
	 *  \return In case the requested resampler is not available, no error is
	 *          returned and the previous active one is retained.
	 */
	LIBRESULT (* SetAudioResamplerType)( eAudioResampler_t nResamplerType );

	/** \brief Reserved
	 *  \ingroup reserved
	 */
	LIBRESULT (* Reserved013 )();


	/** \brief Add a custom parameter for a user binding
	 *
	 *  \ingroup user
	 *
	 *  Use this before RegisterUser() if you want the parameters to appear in
	 *  the registration. Does not need to be before the first registration.
	 *
	 *  The SIP protocol uses URIs to uniquely identify SIP endpoints on the
	 *  network. When a SIP client registers for incoming calls it provides its
	 *  URI to the server. This URI is sent over through the Contact header field.
	 *  It is used for every SIP request but is most important in REGISTER
	 *  request because the server uses this URI to route calls.
	 *
	 *  This function allows manipulation of the Contact URI parameters and the
	 *  Contact parameters (there are two sets).
	 *
	 *  The Contact URI parameter alters the unique identification of the user
	 *  registration. Incoming calls routed back to the client will retain the
	 *  parameter.
	 *
	 *  The Contact non-URI parameters are seen by the server handling the
	 *  registration but are not stored in the server's location database. Incoming
	 *  calls send over the registration will not include these parameters.
	 *
	 *  This function is currently implemented for SIP only but it has this weird
	 *  generic name for two reasons: first, the actual name of the object being
	 *  manipulated is the Contact header field and API users might get confused
	 *  because we already use "contact" for other means. Second, the "Contact"
	 *  header field being manipulated here is used to add/remove user bindings
	 *  on the server.
	 *
	 *  \param[in] UserId The user to configure
	 *  \param[in] uriParam Set to 1 to add a Contact URI parameter (part of the unique ID)
	 *                      Set to 0 to add a Contact parameter (NOT part of the unique ID)
	 *  \param[in] paramName UTF-8 nul-terminated parameter name
	 *  \param[in] paramValue UTF-8 nul-terminated parameter value
	 *
	 *  \sa ClearUserBindingParams()
	 */
	LIBRESULT (* AddUserBindingParam )( UserHandler UserId, int uriParam, const char * paramName, const char * paramValue );

	/** \brief Remove all custom user binding parameters
	 *
	 *  \ingroup user
	 *
	 *  Use this to reset all custom parameters.
	 *
	 *  \param[in] UserId The user to configure
	 *
	 *  \sa AddUserBindingParam()
	 */
	LIBRESULT (* ClearUserBindingParams )( UserHandler UserId );

	/** \brief Sets the video frame callback per call
	 *
	 *  The callback is invoked from a dedicated video processing thread and is not
	 *  part of the WrapperCallbacks structure.  Use this function to set the callback and user data.
	 *
	 *  The callback is limited to the H.263 (and variants) frame format (YUV 420p
	 *  with alignment).
	 *
	 *  \param[in] pCbk The video frame callback
	 *  \param[in] pUserData User data (e.g. the call itself).
	 *
	 *  \sa CallCreateVideo()
	 */
	LIBRESULT (* CallSetVideoFrameIYUVCbk )( CallHandler CallId, pfVideoFrameIYUVCbk2 pCbk, void * const pUserData );

	/** \brief Verifies signed data with a given public key
	 *
	 *
	 *  \param[in] pData Buffer for the data we are going to verify
	 *  \param[in] pDataLen	Lenght of the data buffer.
	 *  \param[in] pSignatureData Buffer for the signature.
	 *  \param[in] SignatureDataLen	Lenght of the signature.
	 *  \param[in] pKey Buffer with public key (PEM)
	 *  \param[in] KeyLen Length of the pKey buffer
	 */
	LIBRESULT (* VerifySignature) ( void * pData, int DataLen, void * pSignatureData, int SignatureDataLen, void * pKey, int KeyLen);

	/** \brief Verifies file signiture using another file for the signiture data with a given public key
	 *
	 *
	 *  \param[in] DataFile The path to the file which we are going to check.
	 *  \param[in] SignatureFile The path to the file containing the signature.
	 *  \param[in] pKey Buffer with public key (PEM)
	 *  \param[in] KeyLen Length of the pKey buffer
	 */
	LIBRESULT (* VerifySignatureFile) ( const char *DataFile, const char *SignatureFile, void * pKey, int KeyLen);

	/** \brief Start activation for an SDK product
	 *
	 *  \ingroup global
	 *
	 *	Starts activation process of the SDK product. Without activation, much of 
	 *	the functions are not allowed and will return L_NOPERM. Activation first 
	 *	tries to use data from a certificate cache file, the path to which is 
	 *  supplied by the user. If the file is available and valid for the current 
	 *  device, it's contents will be used to set the allowed functionalities. If 
	 *  file is invalid OR not present, the SDK will make an HTTP request to a 
	 *  licensing server. On valid response the results from the server will be used
	 *  to configure functionality availability and, if the location supplied by the 
	 *  user in certCacheFile is valid and writable, the results will be stored there
	 *  so that the cache file can be used on next invocation.
	 *
	 *  \param[in] certCacheFile Filename to use for certificate storage. 
	 *  \param[in] username Username to use for authentication to the cert server
	 *  \param[in] password Password to use for authentication to the cert server
	 *  \param[in] devId Device ID of the current device. Used only on android as
	 *		there is no way to get this data without JNI. All other platforms ignore
	 *		this parameter and generate the value internally
	 *
	 *  \sa StopActivation()
	 */
	LIBRESULT (* StartActivationSDK )(	const char * certCacheFile, const char * username, 
										const char * password, const char * devId);

	/** \brief Send a video frame over the network
	 *
	 *  Send an arbitrary-format frame to the video processing thread identified
	 *  by \p pUserData (returned by WrapperCallbacks::onVideoStarted()).
	 *
	 *  The function will copy the buffer into its internal structures and schedule
	 *  it for encoding and transmitting over the video call.  It will return 
	 *  immediately.
	 *
	 *  \param[in] pThreadId The thread id returned by WrapperCallbacks::onVideoStarted()
	 *  \param[in] pBuffer The buffer with data.
	 *  \param[in] bufLen The size of the buffer in bytes
	 *  \param[in] width Frame width in pixels
	 *  \param[in] height Frame height in pixels
	 *  \param[in] format Format of the video frame
	 *
	 *  \sa WrapperCallbacks::onVideoStarted(), CallCreateVideo()
	 */
	LIBRESULT (* VideoSendFrame2 )( void * pThreadId, const void * pBuffer, int bufLen, 
			int width, int height, eVideoFrameFormat_t format );

	/** \brief Sets the video raw frame callback per call
	 *
	 *  The callback is invoked from a dedicated video processing thread and is not
	 *  part of the WrapperCallbacks structure.  Use this function to set the callback and user data.
	 *
	 *  The callback is limited raw encoded bitstream format.
	 *
	 *  \param[in] pCbk The video frame callback
	 *  \param[in] pUserData User data (e.g. the call itself).
	 *
	 *  \sa CallCreateVideo()
	 */
	LIBRESULT (* CallSetVideoFrameRAWCbk )( CallHandler CallId, pfVideoFrameRAWCbk pCbk, void * const pUserData );

	/** \addtogroup push Push notifications
	 *
	 *  @{
	 */

	/** \brief Sends push token to the server
	 *
	 *  Push notifications can be used to lower battery usage on mobile platforms
	 *  by allowing a program to enter sleep mode while having the option to be
	 *  woken up by the network.
	 *
	 *  This function can be used to send a push provider token for the current
	 *  device to a SIP server that supports push notifications.
	 *
	 *  There is a separate document describing the SIP extensions used to transmit
	 *  and receive push-related tokens. TODO: add document.
	 *
	 *  The push token itself is obtained by means outside of the scope of this
	 *  API. The WRAPPER is aware of the platform-specific token formats because it
	 *  has to properly transport them over SIP. For example Apple uses a binary
	 *  string that is better transported with base64 while Microsoft and Google
	 *  use URIs that don't need to be transformed prior to transporting.
	 *
	 *  Because the push token can be an arbitrary binary string there is a parameter
	 *  that specifies its length. If \p pushTokenBufLen is set to zero the
	 *  \p pushTokenBuf will be treated as nul-terminated UTF-8 string. Otherwise
	 *  it will be treated as a byte buffer.
	 *
	 *  The \p serviceUserId is the SIP user that is used to communicate with the
	 *  push relay (push notification aware SIP server).
	 *
	 *  The \p deviceId string is used by the server to cache push tokens. More than
	 *  one device is allowed for the same service user on the server end.
	 *
	 *  If the API user can't provide a device ID an empty or NULL string can
	 *  be used. In this case a random ID will be generated by the Library and
	 *  will be returned in the WrapperCallbacks::onPushTokenSuccess() event.
	 *  The API user is advised to cache this device ID and use it in subsequent
	 *  calls to PublishPushToken().
	 *
	 *  The \p expirySeconds parameter sets the expected life of the push token. If
	 *  the token's lifetime is not known it should be set to zero.
	 *
	 *  \param[in] serviceUserId Service user
	 *  \param[in] deviceId Locally generated device Id
	 *  \param[in] pushFormat Push token format
	 *  \param[in] pushTokenBuf Platform-specific push token
	 *  \param[in] pushTokenBufLen Length in bytes of pushTokenBuf or 0 for nul-terminated strings
	 *  \param[in] expirySeconds Expected life of the push token or 0 for auto
	 *
	 *  \sa WrapperCallbacks::onPushTokenSuccess(),
	 *      WrapperCallbacks::onPushTokenFailure(),
	 *      RequestProviderToken(), RemoveProviderToken()
	 */
	LIBRESULT (* PublishPushToken )( UserHandler serviceUserId, const char * deviceId,
	                                 ePushTokenFormat_t pushFormat, const void * pushTokenBuf,
	                                 int pushTokenBufLen, int expirySeconds );

	/** \brief Request a provider-specific push token
	 *
	 *  Regular push notification can be done with a push notification enabled SIP
	 *  server. This function allows SIP servers that are not aware of the full
	 *  extensions to also benefit from push notifications.
	 *
	 *  Details about how the SIP provider communicates with the push-enabled SIP
	 *  service are described in another document. TODO: add document.
	 *
	 *  This is also done to prevent a regular SIP provider from learning the
	 *  device-specific push token. All push notifications will be done through
	 *  a push relay that can limit notifications per provider.
	 *
	 *  To fully negotiate push tokens for a provider this function needs both
	 *  the user Id of the push service and the user id of the regular provider.
	 *
	 *  \p serviceUserId can be set to INVALID_HANDLE. In this case the WRAPPER will
	 *  wait for PublishPushToken() to be used before negotiating provider-speicifc
	 *  tokens. This is the recommended usage.
	 *
	 *  \p providerUserId specifies the user id for the regular SIP server. During
	 *  the process of negotiating push tokens the WRAPPER might unregister and
	 *  register the \p roviderUserId account. This may result in interruption of
	 *  service (more specifically, incoming calls). To minimize this the API user
	 *  is advised to call this function before the first registration for the
	 *  \p providerUserId with \p serviceUserId set to INVALID_HANDLE.
	 *
	 *  Also to further minimise the service interruption on the \p providerUserId
	 *  account it is best to cache the resulting provider token and re-use it.
	 *
	 *  \p providerId is a random string identifying the provider account for this
	 *  specific device. It should be generated by the API user but if that is
	 *  not convenient it can also be generated by the WRAPPER. It is strongly
	 *  recommended to cache the \p providerId and reuse it in subsequent calls for
	 *  the same account, regardless of who generates it.
	 *
	 *  To ask the WRAPPER to generate a \p providerId set it to the empty string
	 *  or to NULL. WRAPPER always returns the \p providerId in the
	 *  WrapperCallbacks::onProviderTokenSuccess() callback.
	 *
	 *  \p cachedProviderToken is a string that was returned from a previous
	 *  WrapperCallbacks::onProviderTokenSuccess() and was cached by the API
	 *  user. In the very first call there will be no cache so an empty string or
	 *  NULL can be used. It is strongly recommended to cache the provider token
	 *  and provide it for subsequent calls to RequestProviderToken().
	 *
	 *  Provider tokens have a life time controlled by the push notification
	 *  service. The \p expirySeconds parameter suggests a hint to the push
	 *  service about the desired life time but the service can override it.
	 *  The WRAPPER will automatically try to extend the life by refreshing
	 *  the token before it expires. Set to zero to let the WRAPPER and the
	 *  service negotiate the expiration.
	 *
	 *  \param[in] serviceUserId Service account (set to INVALID_HANDLE for now)
	 *  \param[in] providerUserId Provider account
	 *  \param[in] providerId Provider ID (generated locally)
	 *  \param[in] cachedProviderToken Provider Token (generated by the service)
	 *  \param[in] expirySeconds Expiry in seconds for this token or 0 for auto
	 *
	 *  \sa PublishPushToken(), RemoveProviderToken(),
	 *      WrapperCallbacks::onProviderTokenSuccess(),
	 *      WrpaperCallbacks::onProviderTokenFailure()
	 */
	LIBRESULT (* RequestProviderToken )( UserHandler serviceUserId, UserHandler providerUserId,
	                                     const char * providerId, const char * cachedProviderToken,
	                                     int expirySeconds );

	/** \brief Remove provider specific token
	 *
	 *  This function will disable push functionality for a specific account.
	 *  It may need to unregister and subsequently register the \p providerUserId
	 *  account which may result in interrupted service (no incoming calls on that
	 *  specific account).
	 *
	 *  \p serviceUserId can be set to the account that is used with PublishPushToken()
	 *  ot it can be set to INVALID_HANDLE to let WRAPPER handle it automatically.
	 *
	 *  \p providerUserId is the regular SIP account that will have push notifications
	 *  disabled. All the others (if any) will continue to have push notifications.
	 *
	 *  \p providerId is the provider ID used in RequestProviderToken() or returned
	 *  by WrapperCallbacks::onProviderTokenSuccess(). It can be NULL (or empty) to
	 *  ask for the current provider token to be removed, if any such is known.
	 *
	 *  \p cachedProviderToken is the cached provider Token used in
	 *  RequestProviderToken() or returned by WrapperCallbacks::onProviderTokenSuccess().
	 *  It can be NULL or empty to indicate that the token is not known.
	 *
	 *  It is possible to use this function even without previously calling
	 *  RequestProviderToken() in the same session. This functionality is available
	 *  because push notifications and provider token management survive library
	 *  restarts.
	 *
	 *  \param[in] serviceUserId Service account (set to INVALID_HANDLE for now)
	 *  \param[in] providerUserId Provider account
	 *  \param[in] providerId Optional provider Id
	 *  \param[in] cachedProviderToken Optional provider token
	 *
	 *  \sa RequestProviderToken(), PublishPushToken(),
	 *      WrapperCallbacks::onProviderTokenRemoval()
	 */
	LIBRESULT (* RemoveProviderToken )( UserHandler serviceUserId, UserHandler providerUserId,
	                                    const char * providerId, const char * cachedProviderToken );


	/** \brief Reserved
	 *  \ingroup reserved
	 */
	LIBRESULT (* Reserved014 )();
	
	/** @} */

	/** \brief Get codec count
	 *
	 *  \ingroup global
	 *
	 *  This returns the CODEC_COUNT value in the CodecEnum_t enum. This can be used
	 *  in GUI implementations where the codec table is dynamic and is not a copy of
	 *  CodecEnum_t.
	 *
	 *  Due to legacy issues this is not the actual count of working codecs, just
	 *  the number of known codecs. GetCodecCapabilities() must be used to obtain
	 *  per-codec information.
	 *
	 *  \returns The total number of codecs known (including some unsupported codecs)
	 */
	int (* GetCodecCount )( void );

	/** \brief Get codec capabilities
	 *
	 *  \ingroup global
	 *
	 *  Returns the capabilities of a codec. If the codec is not supported, L_FAIL
	 *  will be returned and the output parameters will not be touched.
	 *
	 *  Both audio and video encoders produce a stream which may have a constant bitrate or
	 *  a variable bitrate. The lower and upper bounds for each codec is different.
	 *
	 *  All encoders have a recommended default bitrate which provides a balance between
	 *  output bitrate and decoder quality.
	 *
	 *  Some encoders can work in different bitrates while others will have only a single
	 *  output bitrate. Those that support different bitrates might also support variable
	 *  bitrate mode in which the encoder itself will switch between bitrates on the fily.
	 *
	 *  Some audio encoders can also detect when there is no voice to be sent and stop
	 *  producing packets. The detection is called VAD (voice activity detection) and the
	 *  mode in which the encoder will send no packets when there is no voice activity
	 *  is called DTX (discontinuous transmission).
	 *
	 *  Most video encoders will vary their output based on the motion in the video stream
	 *  that is being encoded.
	 *
	 *  Some encoders are neither video nor audio but are used to transmit other types of
	 *  signals, for example DTMFs.
	 *
	 *  The bit rates described here involve only one direction, specifically the encoder.
	 *  Each encoder is controlled separately so the traffic in the two directions can be
	 *  very different, especially if two different encoders are used for the incoming and
	 *  outgoing streams.
	 *
	 *  The bit rates exposed by this function do not include any protocol overhead. This
	 *  is the raw output of the encoder. In case of SIP/RTP/UDP there is always additional
	 *  overhead of about 21600 bits per second. SRTP (used also by ZRTP) will always have
	 *  additional overhead of 21600 (from RTP) + 3200-4000 bits per second depending on
	 *  authentication tag size. IAX2 involves similar overhead as RTP, although it can be
	 *  smaller, at about 18000 bits per second.
	 *
	 *  \param[in] codecId The codec id/index. This can be CodecEnum_t or any number
	 *                     between 0 and CODEC_COUNT-1. Use GetCodecCount() to obtain
	 *                     the current CODEC_COUNT value or use CODEC_COUNT directly.
	 *  \param[out] minBPS If not 0, points to an integer that will receive the minimum
	 *                     bitrate for this codec's encoder in bits per second. Does not
	 *                     include protocol overhead!
	 *  \param[out] maxBPS If not 0, points to an integer that will receive the maximum
	 *                     bitrate for this codec's encoder in bits per second. Does not
	 *                     include protocol overhead!
	 *  \param[out] codecFlags If not 0, points to an integer that will receive a bitmask
	 *                     of various eCodecFlags_t flags. These describe the encoder's
	 *                     capability of doing DTX or VBR, video and audio support.
	 *  \param[out] friendlyNameBuf If not 0, and if firendlyNameBufLen is not 0, this
	 *                     buffer will receive a nul-terminated UTF-8 string containing a
	 *                     human readable name describing this codec.
	 *  \param[in] friendlyNameBufLen Sets the maximum number of octets, including the
	 *                     terminating zero that can be written to friendlyNameBuf.
	 *
	 *  \returns L_OK if the codec is known and supported
	 *           L_FAIL if the codec is not known or not supported
	 */
	LIBRESULT (* GetCodecCapabilities )( CodecEnum_t codecId, int * minBPS, int * maxBPS, int * defaultBPS, int * codecFlags, char * friendlyNameBuf, int friendlyNameBufLen );

	/** \brief Configures per-user codec settings
	 * \ingroup user
	 *
	 *  Use this function to configure each codec added to a user's codec list. Does not
	 *  work unless the codec has been added for this user by AddUserCodec().
	 *
	 *  \param[in] userId  The user to configure
	 *  \param[in] codecId The codec to configure. Must have been added with AddUserCodec()
	 *                     to the user.
	 *  \param[in] bps     Selects a bitrate for the encoder. Use GetCodecCapabilities()
	 *                     to obtain the valid range.
	 *  \param[in] useDTX  Controls the DTX feature, if supported by the codec.
	 *  \param[in] useVBR  Controls the VBR feature, if supported by the codec.
	 *
	 *  \returns L_OK if the setting was supported and applied successfully.
	 *           L_FAIL if
	 *                  - the user was not found, or
	 *                  - the codec was not supported, or
	 *                  - the codec has not been added to the user's codec list, or
	 *                  - the codec does not support the parameters
	 */
	LIBRESULT (* SetUserCodecParameters )( UserHandler userId, CodecEnum_t codecId, int bps, int useDTX, int useVBR );

	/** \brief Set up the synchronous stop callback for ExternalAudio
	 *
	 *  \ingroup audio
	 *
	 *  This must be used in the new version of ExternalAudio.
	 *
	 *  This call sets the function callback that the WRAPPER must use to stop
	 *  the ExternalAudio and/or wait for the ExternalAudio thread's completion,
	 *  if any.
	 *
	 *  When ExternalAudio is requested by the WRAPPER it will issue an
	 *  onExternalAudioRequested() callback. At some point after this callback
	 *  the ExternalAudio is supposd to call this function and then start
	 *  calling ExternalAudioFrame().
	 *
	 *  The pCbk function will be called by the WRAPPER when it wants the
	 *  ExternalAudio to stop calling the ExternalAudioFrame() function.
	 *  WRAPPER will pass pUserData to the pCbk function.
	 *
	 *  The pCbk function must wait (must block) until it has made sure the
	 *  ExternalAudioFrame() call will no longer be invoked.
	 */
	LIBRESULT (* SetExternalAudioSyncStopCallback )( pfExternalAudioSyncStopCbk pCbk, void * pUserData );

	/** \brief Create a new recording 
	 *
	 *  \ingroup soundrec
	 *
	 *  Creates a new sound recording that will record the microphone into
	 *  a memory buffer which later will be available as a sound object or
	 *  as a direct sample buffer.
	 *
	 *  This version of the function allows setting the sampling rate and
	 *  channel count for the recording. Note that the sample format is still
	 *  16 bit signed PCM in the local endian. Only PPC on Mac for now does
	 *  big endian; the rest are little endian.
	 *
	 *  Note that currently only mono is implemented with stereo and
	 *  multichannel recording coming in later versions.
	 *
	 *  Note also that although not all sampling rates are supported by
	 *  each hardware WRAPPER will try to resample the stream if needed to
	 *  achieve the desired format. If no resampling can be done for some
	 *  very exotic sampling rates, the function will return L_FAIL.
	 *  The WRAPPER guarantees the following sampling rates regardless of
	 *  hardware limitations: 8000, 16000, 32000, 44100, 48000. Some other
	 *  might also work depending on the resampler's capabilities.
	 *
	 *  Unlike AddRecording() this function does not use milliseconds for
	 *  the recording size. Instead it uses samples (ignoring the channel
	 *  count). This allows for more precise control. The sample count is
	 *  across all channels. Use this formula to convert to seconds (use
	 *  float or fixed point for precision):
	 *
	 *  seconds = maxLengthSamples / sampleRateHz
	 *
	 *  Or this one to convert to milliseconds (again, use float or fixed
	 *  point for precision):
	 *
	 *  milliseconds = ( maxLengthSamples * 1000 ) / sampleRateHz
	 *
	 *  \param[in] sampleRateHz Sampling rate for the recording (8000, 16000, etc)
	 *  \param[in] channels Channel count for the recording (1 = mono)
	 *  \param[in] maxLengthSamples Maximum recording length in samples
	 *  \param[out] pRecordingId The resulting recording id
	 *
	 *  \returns L_OK if the recording is ready to be started
	 *           L_FAIL if:
	 *             - the combination of sampleRateHz and channels is unsupported
	 *             - there is not enough memory to allocate continuous buffer for the recording
	 *             - the sound system is not properly initialized
	 *  \sa AddRecording(), StartRecording(), StopRecording(), GetRecordingBuffer(),
	 *      CreateSoundFromRecording(), RemoveRecording()
	 */
	LIBRESULT (* AddRecording2 )( int sampleRateHz, int channels, int maxLengthSamples, RecordingHandler * pRecordingId );

	/** \brief Load music service file with error reporting
	 *
	 *  \ingroup global
	 *
	 *  Same as LoadMusicServiceFile() but in case of error (i.e. in case this
	 *  function returns L_FAIL), causeCode will get the error code. For best
	 *  results detailed errors should be enabled.
	 *
	 *  The format of the file must be WAV in the PCM or PCM extended subtype,
	 *  the frequency can be almost anything (most standard frequencies are
	 *  supported), the bits per sample can be 8, 16, 24 or 32 and the channels
	 *  can be anything but they might get downmixed to mono automatically.
	 *
	 *  \param[in] fileNameUtf8 UTF-8 file name of the music file (WAV format)
	 *
	 *  \returns L_OK if the file is loaded. L_FAIL if there is an error or
	 *           if the file format is not supported. In case of L_FAIL,
	 *           causeCode will get the error code.
	 *
	 *  \sa SetUserMusicService(), SetCallMusicService(), LoadMusicServiceFile()
	 */
	LIBRESULT (* LoadMusicServiceFile2 )( const char * fileNameUtf8, int * causeCode );

	/** \brief Load a sound from a WAV file
	 *
	 *  \ingroup sound
	 *
	 *  Loads a sound from a WAV file. The sound will inherit the WAV file's format
	 *  with the only exception that 8-bit, 24-bit and 32-bit PCM samples will be
	 *  automatically converted to 16-bit while the sound is being loaded.
	 *
	 *  The frequency and channel count will be left unchanged.
	 *
	 *  While playing the resulting sound through StartSound() or StartPlayback()
	 *  it can undergo resampling (to match frequencies) or remixing (to match
	 *  the channels) based on the current audio settings.
	 *
	 *  The loading can be done asynchronously in a separate WRAPPER thread by
	 *  using the async parameter. In case async is non-zero the function will
	 *  only check the WAV file and its format and will try to allocate the
	 *  needed memory. It will return the SoundHandler for the new sound but
	 *  it will not be playable until WrapperCallbacks::onSoundLoadCompleted()
	 *  has been invoked.
	 *
	 *  \param[in] utf8Name Name of the WAV file to load from, UTF-8
	 *  \param[in] bRepeat 1 if the sound should be looped when played
	 *  \param[in] pauseMs If the sound is to be looped, the amount of silence
	 *             in milliseconds between each loop.
	 *  \param[in] async If 1, load the file in another thread, returning
	 *             from this function immediately
	 *  \param[out] handle The resulting handle
	 *  \param[out] causeCode In case of error, the error code
	 *
	 *  \return L_OK on success or L_FAIL on error (check causeCode for more details)
	 *
	 *  \sa AddSound(), StartSound(), StopSound(), RemoveSound(), GetSoundFormat(),
	 *      WrapperCallbacks::onSoundLoadCompleted()
	 */
	LIBRESULT (* AddSoundFromWav )( const char * utf8Name, int bRepeat, int pauseMs, int async, SoundHandler * handle, int * causeCode );

	/** \brief Get sound format and duration
	 *
	 *  \ingroup sound
	 *
	 *  Obtains sound parameters for any sound with a valid handle,
	 *  no matter how it was created.
	 *
	 *  The length of the sound is natively expressed in samples in the same
	 *  frequency as the sound. The approxLengthMs receives this length
	 *  converted to 1000 Hz (i.e. the units are milliseconds) but because
	 *  virtually all sounds run at at least 8000 Hz there will be loss
	 *  in precision. Thus the name "approximate length in milliseconds".
	 *
	 *  For full precision please use the lengthSamples output parameter instead.
	 *
	 *  \param[in] soundId The sound handle
	 *  \param[out] freqHz Frequency in hertz
	 *  \param[out] lengthSamples Length in samples in the freqHz frequency
	 *  \param[out] approxLengthMs Approximate length in milliseconds
	 *  \param[out] channelCount Number of channels
	 *  \param[out] repeat 1 if the sound is configured to repeat when played
	 *  \param[out] pauseMs Pause in milliseconds, only if repeat is 1
	 *
	 *  \return L_OK if the sound exists L_FAIL if no such sound exists
	 *
	 *  \sa AddSound(), AddSoundFromWav(), CreateSoundFromRecording()
	 */
	LIBRESULT (* GetSoundFormat )( SoundHandler sndId, int * freqHz, int * lengthSamples, int * approxLengthMs, int * channelCount, int * repeat, int * pauseMs );
	
	
	/** \brief Starts a latency measurement test
	 * 
	 * Starts a latency measurement test on the device. The test consists of 
	 * playing back a short tone while recording the microphone sound at the 
	 * same time and then finding the minimum and maximum cross-correlation 
	 * values between the input and output sample buffers. As the measurement
	 * and calculations take some time (in the order of tens of seconds0, the 
	 * result will be delivered in a onLatencyTestCompleted callback. 
	 * 
	 *  \param[in] sampleRata Sample Rate to use during the tests in Hz. Allowed
	 *				sample rates depend on the actual hardware, but usually 8000,
	 *				44100 and 48000 are available	 *		
	 *  \param[in] bufferSizeInSamples audio buffer size in number of frames to
	 *				use during the test. Usually a good start is 20ms=>882 
	 *				at 44100Hz
	 *  \param[in] maxTimeMs maximum time in ms to record. Setting it too high 
	 *				will significantly increase the test time. Setting it too 
	 *				low will result in the return signal not being captured and 
	 *				wrong results. Typical value to use on high-end android 
	 *				devices is 300, on low-end - 1000 
	 * \param[in] stereo Set to 0 to use mono mode during the tests. Set to 1
	 *				to use stereo. Even in stereo mode, only one channel is 
	 *				actually used to calculate the latency.
	 *	\return	L_OK if the test was successfully started
	 * 
	 *  \sa WrapperCallbacks::onLatencyTestCompleted()
	 */
	LIBRESULT (* StartLatencyTest) (int sampleRate, int bufferSizeInSamples, int maxTimeMs, int stereo);

	/** \brief Generate a random UUID
	 *
	 *  \ingroup init
	 *
	 *  Generates a valid RFC 4122 (RFC 2141) UUID URN that can be used for
	 *  RFC 5626 (SIP Outbound).
	 *
	 *  The result is a string in the format
	 *
	 *  "urn:uuid:xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
	 *
	 *  or the format
	 *
	 *  "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
	 *
	 *  depending on the \p noPrefix parameter. The prefix is there for easier
	 *  use of SetUserSipInstance().
	 *
	 *  In case the "urn:uuid:" prefix is not desired, put 1 to the \p noPrefix
	 *  parameter. The resulting string will be shorter and will contain only
	 *  the random part.
	 *
	 *  If \p noPrefix is set to 0, the resulting string can be given directly
	 *  to SetUserSipInstance().
	 *
	 *  \param[in] noPrefix Set to 0 for full URN, 1 for a UUID without prefix.
	 *  \param[out] outBuffer Output buffer to write the string to
	 *  \param[in] bufLen Available bytes in the buffer (64 bytes or more are
	 *                    sufficient)
	 *
	 *  \sa SetUserSipInstance()
	 */
	LIBRESULT (* GenerateUUID )( int noPrefix, char * outBuffer, int bufLen );


	/** \brief Configures RFC2141 URN for SIP registrations
	 *
	 *  \ingroup user
	 *
	 *  Only valid for SIP user accounts.
	 *
	 *  This function configures the SIP instance URN used for SIP outbound
	 *  (RFC 5626). It MUST be a valid RFC 2141 URN and it SHOULD be a valid
	 *  RFC 4122 UUID URN in the format "urn:uuid:xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
	 *  where 'x' is a hexadecimal number. It can be optionally in angle
	 *  brackets.
	 *
	 *  Other URN schemes can be used. The recommended one is urn:uuid but
	 *  most servers will treat the string as a sequence of bytes and will not
	 *  care if the scheme is different.
	 *
	 *  Use GenerateUUID() to create a random UUID. It will include the scheme
	 *  prefix (urn:uuid:) and can be used directly in this function. Other ways
	 *  of generating/obtaining a URN/UUID are allowed but the URN prefix is
	 *  REQUIRED by the standard.
	 *
	 *  Global URN can be applied for all SIP accounts by setting userdId to
	 *  INVALID_HANDLE. SIP Outbound MUST still be enabled individually for each
	 *  account.
	 *
	 *  The URN SHOULD be stored locally and re-used after restarts. The API
	 *  user has the responsibility of using the same URN between restarts.
	 *
	 *  \param[in] userId The SIP user id to configure or INVALID_HANDLE to
	 *                    configure the URN globally.
	 *  \param[in] instanceURN The RFC 2141 URN to be used in RFC 5626 (SIP outbound)
	 *
	 *  \sa GenerateUUID()
	 */
	LIBRESULT (* SetUserSipInstance )( UserHandler userId, const char * instanceURN );

	/** \brief Configures RFC 5626 (SIP Outbound) for a SIP user
	 *
	 *  \ingroup user
	 *
	 *  Only valid for SIP user accounts.
	 *
	 *  This function enables or disables RFC 5626 for SIP users. The default
	 *  setting for new SIP user account is "disabled".
	 *
	 *  The user MUST also have a valid SIP Instance URN configured by
	 *  SetUserSipInstance(). It is OK to configure a global SIP Instance URN
	 *  once by configuring it to INVALID_HANDLE.
	 *
	 *  The user MUST also have an Outbound Proxy configured in AddUser().
	 *
	 *  \param[in] userId The SIP user id to configure
	 *  \param[in] enabled Set 1 to enable, 0 to disable SIP Outbound.
	 */
	LIBRESULT (* SetUserSipOutbound )( UserHandler userId, int enabled );

	/** \brief Replace user registration
	 *
	 *  \ingroup user
	 *
	 *  This function is currently available for SIP users only.
	 *
	 *  For protocols such as IAX2 there is no "binding" and this operation
	 *  does not apply. For those types of protocols the function will
	 *  internally call RegisterUser() and then return its status.
	 *
	 *  This function will first try to cleanly remove any active SIP bindings
	 *  on the registrar server that were put by this instance of the library.
	 *
	 *  Then it will try to create a new registration binding on the server.
	 *
	 *  This is mostly useful when switching network types and an IP address
	 *  change has been detected. This function will try to remove the old
	 *  (stale) binding that is currently active on the server and replace it
	 *  with a "fresh" one done from the new IP address.
	 *
	 *  A better approach might be to use SIP Outbound (RFC 5626) but that
	 *  requires server support.
	 *
	 *  \param[in] userId The SIP user to re-register
	 *
	 *  \sa RegisterUser(), UnregisterUser()
	 */
	LIBRESULT (* ReplaceUserRegistration )( UserHandler userId );

	/** \brief Refresh call
	 *
	 *  \ingroup call
	 *
	 *  This function is currently available for SIP calls only.
	 *
	 *  SIP:
	 *
	 *    This functions will do a "refresh" re-INVITE and will also
	 *    restart any active RTP streams. Active streams are those that
	 *    have active audio or video and are not "on hold" in SIP (SDP).
	 *
	 *  If the call state is not suitable for a refresh (possibly during
	 *  an active SIP transaction, while on hold, during any type of
	 *  transfer, during video (re)negotiation, etc., the function will
	 *  return L_FAIL without doing anything (the call will be unaffected).
	 *
	 *  If the functions returns L_OK an WrapperCallbacks::onCallRefreshCompleted()
	 *  will be invoked when the protocol-level refresh is done. The callback
	 *  is suitable for chaining requests.
	 *
	 *  \param[in] callId The ID of the call to refresh
	 *
	 *  \sa WrapperCallbacks::onCallRefreshCompleted()
	 */
	LIBRESULT (* CallRefresh )( CallHandler callId );

	/** \brief Globally enable IPv6
	 *
	 *  \ingroup init
	 *
	 *  Globally enable IPv6 support. Once enabled all networking will
	 *  try to go through IPv6 first, if possible, before trying IPv4.
	 *
	 *  Individual adjustment is still possible to some extent.
	 *
	 *  To use IPv6 for TLS this function must be called before
	 *  SipInitTls() and SetUserTlsConfig().
	 *
	 *  To disable IPv6 the library must be restarted.
	 */
	LIBRESULT (* InitIPv6 )();

	/** \brief Force IPv4 operation for a user
	 *
	 *  \ingroup user
	 *
	 *  Force IPv4 operation for a specific user. Useful for disabling
	 *  the IPv6 autodetection for SIP accounts.
	 *
	 *  The autodetection is for negotiating RTP (via SDP).
	 *
	 *  This also disables TLSv6 when the SIP user has individual
	 *  TLS server running. For this to happen this must be called
	 *  before SetUserTlsConfig().
	 *
	 *  \param[in] userId The user to configure
	 *  \param[in] forceIPv4 Set to 1 to force IPv4,
	 *                       0 to allow automatic detection of IPv6
	 */
	LIBRESULT (* SetUserForceIPv4 )( UserHandler userId, int forceIPv4 );

	/** \brief Encrypts data using AES-CBC and performs base64 encoding if parameter is set.
	 *
	 *  \param[in] pIn Input buffer (data to be encrypted)
	 *  \param[out] pOut Output buffer, should be twice as big as the input buffer. Result is zero terminated
	 *  \param[in] len Input buffer size in bytes
	 *  \param[in] key Key
	 *  \param[in] keylen Key size
	 *  \param[in] b64encode If set to 1 data will be base64 encoded,
	 *                       0 will skip encoding
	 *
	 *  \sa AESEncryptDataInCBCMode2()
	 */
	LIBRESULT (* AESEncryptDataInCBCMode2 )( const unsigned char * pIn, unsigned char  * pOut,
	                                         unsigned long len, const unsigned char * key,
	                                         AesKeyLength_t keylen, int b64encode );

	/** \brief Decrypts data using AES-CBC. If the parameter is set base64 decoding will be performed prior decrypting.
	 *
	 *  Unlike AESDecryptDataInCBCMode2() this variant supports any base64 format.
	 *  It is recommended to use this instead of AESDecryptDataInCBCMode2().
	 *
	 *  \param[in] pIn Input buffer (data to be decrypted)
	 *  \param[out] pOut Output buffer, should be as big as the input buffer
	 *  \param[in] len Input buffer size in bytes
	 *  \param[in] key Key
	 *  \param[in] keylen Key size
	 *  \param[in] b64decode If set to 1 base64 decoding will be performed,
	 *                       0 will skip decoding
	 *
	 *  \sa AESDecryptDataInCBCModePure()
	 */
	LIBRESULT (* AESDecryptDataInCBCModePure2 )( unsigned char * pIn, unsigned char * pOut,
	                                             unsigned long len, const unsigned char * key,
	                                             AesKeyLength_t keylen, int b64decode );

	/** \brief Decrypts data using AES-CBC (base64 format is restricted). If the parameter is set base64 decoding will be performed prior decrypting.
	 *
	 *  \param[in] pIn Input buffer (data to be decrypted)
	 *  \param[out] pOut Output buffer, should be as big as the input buffer
	 *  \param[in] len Input buffer size in bytes
	 *  \param[in] key Key
	 *  \param[in] keylen Key size
	 *  \param[in] b64decode If set to 1 base64 decoding will be performed,
	 *                       0 skips decoding
	 *
	 *  \sa AESDecryptDataInCBCMode2
	 */
	LIBRESULT (* AESDecryptDataInCBCMode2 )( unsigned char * pIn, unsigned char * pOut,
	                                         unsigned long len, const unsigned char * key,
	                                         AesKeyLength_t keylen, int b64decode );

	/** \brief Encrypts data using AES-OFB and performs base64 encoding if the parameter is set.
	 *
	 *  \param[in] pIn Input buffer (data to be encrypted)
	 *  \param[out] pOut Output buffer, should be twice as big as the input buffer. Result is zero terminated
	 *  \param[in] len Input buffer size in bytes
	 *  \param[in] key Key
	 *  \param[in] keylen Key size
	 *  \param[in] b64encode If set to 1 data will be base64 encoded,
	 *                       0 will skip encoding
	 *  \param[out] outlen Encrypted data size
	 *
	 *  \sa AESEncryptDataInCBCMode2()
	 */
	LIBRESULT (* AESEncryptDataInOFBMode )( const unsigned char * pIn, unsigned char  * pOut,
	                                       unsigned long len, const unsigned char * key,
	                                       AesKeyLength_t keylen, int b64encode, int* outlen );

	/** \brief Decrypts data using AES-OFB. If the parameter is set base64(format is not restricted) decoding will be performed prior decrypting.
	 *
	 *  \param[in] pIn Input buffer (data to be decrypted)
	 *  \param[out] pOut Output buffer, should be as big as the input buffer
	 *  \param[in] len Input buffer size in bytes
	 *  \param[in] key Key
	 *  \param[in] keylen Key size
	 *  \param[in] b64decode If set to 1 base64 decoding will be performed,
	 *                       0 skips decoding
	 *  \param[out] outlen Decrypted data size
	 *
	 *  \sa AESDecryptDataInCBCMode2

	 */
	LIBRESULT (* AESDecryptDataInOFBMode )( unsigned char * pIn, unsigned char * pOut,
	                                        unsigned long len, const unsigned char * key,
	                                        AesKeyLength_t keylen, int b64decode, int* outlen );

	/** @} */
} WrapperContext;

/** \brief Loads the WrapperContext
 *
 *  Fills the structure with points to the library's functions.
 *
 *  \param[in,out] ctx The wrapper context. Set ctxVersion before calling LoadWrapperContext()
 */
EXPORT LIBRESULT LoadWrapperContext( WrapperContext * ctx );

#if( defined( __cplusplus ) )
}
#endif

#endif
