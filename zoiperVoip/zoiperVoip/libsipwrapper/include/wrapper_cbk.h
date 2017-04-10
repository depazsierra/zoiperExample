#if( !defined( _WRAPPER_CALLBACKS_H_INCLUDED_ ) )
#define _WRAPPER_CALLBACKS_H_INCLUDED_

#if( defined( __cplusplus ) )
extern "C" {
#endif

/** \brief Current wrapper callback version.
 *
 *  New versions of the structure will be binary compatible with older
 *  version.  New function pointers will be added at the end of the
 *  structure.
 */
#define WRAPPER_CALLBACK_VERSION 23

/** \brief Wrapper Callbacks
 *
 *  Contains pointers to event handling functions.
 */
typedef struct {
	/** \brief Structure version
	 *
	 *  Will be filled in by WrapperContext::InitCallbackTable()
	 *
	 *  \sa WrapperContext::InitCallbackTable()
	 */
	int cbkVersion;

	/** \addtogroup callbacks Library callbacks
	 *
	 *  Callbacks for all the events that happen during the normal
	 *  operation of the library.  Use WrapperContext::PollEvents()
	 *  to process the event queue in your own thread.
	 *
	 *  @{
	 */

	/** \addtogroup usercbk User account callbacks
	 *
	 *  @{
	 */

	/** \brief User account has registered
	 *
	 *  The user account has successfully registered to the server.
	 *
	 *  \param[in] userId The user that has registered
	 *  \param[in] pAor Full address of record for the user
	 *  \param[in] newMsg New messages in the mailbox or -1 if not known yet
	 *  \param[in] oldMsg Old messages in the mailbox or -1 if not known yet
	 *
	 *  \sa WrapperContext::RegisterUser(), onUserUnregistered(),
	 *      onUserRegistrationFailure(), onUserRegistrationRetrying(),
	 *      onMessagesWaiting()
	 */
	void (* onUserRegistered )( UserHandler userId, const char * pAor, int newMsg, int oldMsg );

	/** \brief User account has been unregistered
	 *
	 *  \param[in] userId The user that was unregistered
	 *
	 *  \sa WrapperContext::UnregisterUser(), WrapperContext::RegisterUser()
	 */
	void (* onUserUnregistered )( UserHandler userId );

	/** \brief User reigstration or unregistration failure
	 *
	 *  Sent for both registration and unregistration failures.  Check the
	 *  \p isRegister parameter.
	 *
	 *  \param[in] userId The user
	 *  \param[in] isRegister 1 for registration failure, 0 for unregistration failure
	 *  \param[in] causeCode The cause code
	 *
	 *  \sa onUserRegistered(), onUserUnregistered(), onUserRegistrationRetrying(),
	 *      WrapperContext::RegisterUser(), WrapperContext::UnregisterUser()
	 */
	void (* onUserRegistrationFailure )( UserHandler userId, int isRegister, int causeCode );

	/** \brief Registration is going to be retried
	 *
	 *  Registration or unregistration is going to be retried automatically
	 *  by the library after the specified time.
	 *
	 *  \param[in] UserId The user
	 *  \param[in] IsRegistering 1 = registering, 0 = unregistering
	 *  \param[in] RetrySeconds The (un)registration will be retried in this many seconds
	 *             approximately (the event could have been in the queue for unknown
	 *             period already)
	 *
	 *  \sa onUserRegistered(), onUserUnregistered(), onUserRegistrationFailure(),
	 *      WrapperContext::RegisterUser(), WrapperContext::UnregisterUser()
	 */
	void (* onUserRegistrationRetrying )( UserHandler UserId, int IsRegistering, int RetrySeconds );

	/** \brief Messages Waiting
	 *
	 *  This is a general identification of available messages optionally with
	 *  more specific information.
	 *
	 *  \param[in] UserId The user that has messages waiting on the server
	 *  \param[in] hasMsg 0/1 - if the user has any messages in the mailbox
	 *  \param[in] newMsg -1 for unknown or >= 0 for a message count
	 *  \param[in] oldMsg -1 for unknown or >= 0 for a message count
	 *  \param[in] newUrgentMsg -1 for unknown or >= 0 for a message count
	 *  \param[in] oldUrgentMsg -1 for unknown or >= 0 for a message count
	 *  \param[in] pMailbox The number to dial for voice mail (or 0 if unknown)
	 *
	 *  \sa WrapperContext::RequestMessagesWaiting(), WrapperContext::AddUser()
	 */
	void (* onMessagesWaiting )( UserHandler UserId, int hasMsg, int newMsg, int oldMsg,
	                             int newUrgentMsg, int oldUrgentMsg, const char * pMailbox );

	/** @} */

	/** \addtogroup callcbk Call callbacks
	 *
	 * @{
	 */

	/** \brief Outgoing call created
	 *
	 *  This is mainly a confirmation that the outgoing call was created.
	 *
	 *  \param[in] UserId The user account from which the call originates
	 *  \param[in] CallId The call created by WrapperContext::CallCreate
	 *  \param[in] pPeer The callee (same as the one passed to WrapperContext::CallCreate)
	 *
	 *  \sa WrapperContext::CallCreate()
	 */
	void (* onCallCreate )( UserHandler UserId, CallHandler CallId, const char * pPeer );
	
	/** \brief New incoming call
	 *
	 *  Received a new incoming call for a known user identified by \p UserId.
	 *  The new call handler is given in \p CallId.
	 *
	 *  The caller's chosen display name is put into \p pPeer.  The caller's
	 *  actual user name (or number/extension) that can be called back by
	 *  using WrapperContext::CallCreate() is put into \p pPeerNumber.
	 *  For SIP, pPeerURI contains the full SIP URI of the caller.  The \p pDNID
	 *  is set to the original dialled number which could be different from the
	 *  user account's number.
	 *
	 *  For SIP, \p autoAnswerSeconds is filled from the Call-Info header field,
	 *  answer-after parameter.  If this parameter is not present the value of
	 *  \p autoAnswerSeconds will be -1.  It is used by some 3PCC (third party
	 *  call control) solutions to force the client to auto-answer the call after
	 *  the specified period (could be 0) in seconds.
	 *
	 *  \param[in] UserId The user account that received the call
	 *  \param[in] CallId The new call handler
	 *  \param[in] pPeer Display name of the caller
	 *  \param[in] pPeerNumber Caller number (usually can be called back)
	 *  \param[in] pPeerURI Protocol-specific URI
	 *  \param[in] pDNID Dialed number (what the caller actually dialed to reach us)
	 *  \param[in] autoAnswerSeconds (SIP only) if >= 0, hint to auto-answer in this
	 *             many seconds
	 *
	 *  \sa onCallCreate(), WrapperContext::CallCreate(), onUnknownCall(),
	 *      WrapperContext::CallAccept(), WrapperContext::CallReject(),
	 *      WrapperContext::CallRinging()
	 */
	void (* onCallCreated )( UserHandler UserId, CallHandler CallId, const char * pPeer,
	                         const char * pPeerNumber, const char * pPeerURI, const char * pDNID,
	                         int autoAnswerSeconds );

	/** \brief Call accepted
	 *
	 *  The callback will be invoked when a call is connected (established).
	 *
	 *  \param[in] CallId The call
	 *  \param[in] codec The negotiated codec
	 *  \param[in] dir The direction of the call
	 *
	 *  \sa WrapperContext::CallCreate(), onCallCreate(), onCallCreated(),
	 *      onCallEarlyMedia()
	 */
	void (* onCallAccepted )( CallHandler CallId, CodecEnum_t codec, eCallDirection_t dir );

	/** \brief Outgoing call has early media
	 *
	 *  The callback will be invoked if an outgoing call detects early media.
	 *  If the microphone is not muted audio will go both ways.  During this
	 *  stage a call cannot be normally put on hold.  Using
	 *  WrapperContext::CallHold() will result in muting the hold.
	 *
	 *  \param[in] CallId The outgoing call that got early media
	 *  \param[in] codec The negotiated codec
	 *
	 *  \sa WrapperContext::CallCreate(), onCallAccepted(), onCallRinging()
	 */
	void (* onCallEarlyMedia )( CallHandler CallId, CodecEnum_t codec );

	/** \brief Call codec negotiated
	 *
	 *  \param[in] CallId The call
	 *  \param[in] codec The negotiated codec
	 *
	 *  \sa onCallAccepted(), onCallEarlyMedia(), onCallCodecChanged()
	 */
	void (* onCallCodecNegotiated )( CallHandler CallId, CodecEnum_t codec );

	/** \brief Outgoing call is ringing
	 *
	 *  The remote peer is ringing.  This purely is a signaling event, no
	 *  audio is sent.  Sometimes a server can switch to early media and
	 *  start sending ring back tones.  This will be done through the
	 *  onCallEarlyMedia() and onCallRinging() might not be called.  The
	 *  library will not automatically play ring back tones.  This can be
	 *  achieved by using the \ref sound "sound functions".
	 *
	 *  \param[in] CallId Outgoing call that is ringing
	 *
	 *  \sa WrapperContext::CallCreate(), onCallEarlyMedia(),
	 *      WrapperContext::AddSound(), WrapperContext::StartSound()
	 */
	void (* onCallRinging )( CallHandler CallId );

	/** \brief Call was rejected
	 *
	 *  The call was rejected by the remote peer or negotiation has failed
	 *  in the library.  Happens for outgoing calls.
	 *
	 *  \param[in] CallId The call that was rejected
	 *  \param[in] causeCode Cause of the rejection
	 *
	 *  \sa onCallAccepted(), onCallHangup(), onCallFailure()
	 */
	void (* onCallRejected )( CallHandler CallId, int causeCode );

	/** \brief Call was put on hold by the remote peer
	 *
	 *  The remote peer has put us on hold.  It is possible to put a call
	 *  on hold from both sides.  We will always enforce the hold if
	 *  WrapperContext::CallHold() was called.
	 *
	 *  \param[in] CallId Call that was put on hold
	 *
	 *  \sa onCallUnhold(), WrapperContext::CallHold()
	 */
	void (* onCallHold )( CallHandler CallId );

	/** \brief Call was retrieved by the remote peer
	 *
	 *  The call was retrieved by the remote peer.  This will happen only
	 *  if the call was originally put on hold by the remote peer.  The
	 *  library does not allow the remote end to retrieve a call that was
	 *  put on hold by us (by any means, even muting the sound streams or
	 *  ultimately hanging up the call).
	 *
	 *  \param[in] CallId The call that was retrieved
	 *
	 *  \sa onCallHold(), WrapperContext::CallUnhold()
	 */
	void (* onCallUnhold )( CallHandler CallId );

	/** \brief Call was hung up
	 *
	 *  Call was hung up.
	 *
	 *  \param[in] CallId The call that was hung up
	 *  \param[in] causeCode Cause of the hang up
	 *
	 *  \sa WrapperContext::CallHangup(), onCallRejected(), onCallFailure()
	 */
	void (* onCallHangup )( CallHandler CallId, int causeCode );

	/** \brief Transfer request received
	 *
	 *  Received a transfer request over a call.  The library can handle the
	 *  transfer request automatically, it just needs to be accepted or rejected.
	 *
	 *  The number to transfer to (and display name and optionally an URI, depending
	 *  on the protocol) is given for informational purposes (or to help make the
	 *  decision to accept or reject the transfer).
	 *
	 *  Use WrapperContext::TransferAccept() or WrapperContext::TransferReject()
	 *  to either accept or reject the transfer request.
	 *
	 *  \param[in] CallId The call on which the transfer request came
	 *  \param[in] pTransferName The name of the transfer target (given by the
	 *             remote peer)
	 *  \param[in] pTransferNumber The number of the transfer target (this is the
	 *             key part)
	 *  \param[in] pTransferURI Optionally, protocol dependant URI of the transfer
	 *             target
	 *
	 *  \sa onCallTransferSucceeded(), onCallTransferFailure(),
	 *      WrapperContext::TransferAccept(), WrapperContext::TransferReject()
	 */
	void (* onCallTransferStarted )( CallHandler CallId, const char * pTransferName,
	                                 const char * pTransferNumber, const char * pTransferURI );

	/** \brief Transfer succeeded
	 *
	 *  The transfer has succeeded.
	 *
	 *  \param[in] CallId The call that was successfully transferred
	 *
	 *  \sa onCallTransferStarted(), onCallTransferFailure(),
	 *      WrapperContext::TransferAccept(), WrapperContext::TransferReject()
	 */
	void (* onCallTransferSucceeded )( CallHandler CallId );

	/** \brief Transfer failed
	 *
	 *  The trasnfer has failed.
	 *
	 *  \param[in] CallId The call that failed to transfer
	 *  \param[in] causeCode Code with which the transfer failed
	 *
	 *  \sa onCallTransferStarted(), onCallTransferSucceeded(),
	 *      WrapperContext::TransferAccept(), WrapperContext::TransferReject()
	 */
	void (* onCallTransferFailure )( CallHandler CallId, int causeCode );

	/** \brief Received DTMF from the remote peer
	 *
	 *  \param[in] CallId The call on which a DTMF signal was received
	 *  \param[in] DTMF The DTMF signal
	 *
	 *  \sa WrapperContext::CallSendDtmf(), onCallDTMFResult()
	 */
	void (* onCallRecvDTMF )( CallHandler CallId, eDtmfCode_t DTMF );

	/** \brief Result for sending DTMF
	 *
	 *  If WrapperContext::CallSendDtmf() returned success this callback will
	 *  be used to deliver the final result of the DTMF transmission.
	 *
	 *  \param[in] CallId The call on which we tried to send a DTMF
	 *  \param[in] lRes The final result of the transmission
	 *
	 *  \sa WrapperContext::CallSendDtmf(), onCallRecvDTMF()
	 */
	void (* onCallDTMFResult )( CallHandler CallId, LIBRESULT lRes );

	/** \brief Codec renegotiated
	 *
	 *  \param[in] CallId The call which had its codec renegotiated
	 *  \param[in] codec The new codec
	 *
	 *  \sa onCallCodecNegotiated()
	 */
	void (* onCallCodecChanged )( CallHandler CallId, CodecEnum_t codec );

	/** \brief Call failed
	 *
	 *  The call negotiation (or renegotiation) has failed and it will be
	 *  destroyed.
	 *
	 *  \param[in] CallId The call
	 *  \param[in] causeCode The cause of the failure
	 *
	 *  \sa onCallHangup(), onCallRejected()
	 */
	void (* onCallFailure )( CallHandler CallId, int causeCode );

	/** \brief Received incoming call for an unknown user
	 *
	 *  An incoming call has arrived but the library was unable to match it
	 *  to any of the local users added by WrapperContext::AddUser().
	 *
	 *  The library creates a temporary user for this call and uses it
	 *  internally.  The call will proceed as a normal incoming call if
	 *  accepted.
	 *
	 *  \param[in] CallId The new call handler
	 *  \param[in] pPeer Display name of the caller
	 *  \param[in] pPeerNumber Caller number (usually can be called back)
	 *  \param[in] pPeerURI Protocol-specific URI
	 *  \param[in] pDNID Dialed number (what the caller actually dialed to reach us)
	 *
	 *  \sa onCallCreated(), onCallCreate(), WrapperContext::CallCreate(),
	 *      WrapperContext::CallAccept(), WrapperContext::CallReject(),
	 *      WrapperContext::CallRinging()
	 */
	void (* onUnknownCall )( CallHandler CallId, const char * pPeer, const char * pPeerNumber, const char * pPeerURI, const char * pDNID );

	/** \brief Received URL (IAX2)
	 *
	 *  \param[in] CallId The call on which the URL was received
	 *  \param[in] pURL The URL
	 *
	 *  \sa onCallRecvText()
	 */
	void (* onCallRecvURL )( CallHandler CallId, const char * pURL );

	/** \brief Received text message 
	 *
	 *  \param[in] CallId The call on which the text message was received
	 *  \param[in] pTxt The text message, UTF-8
	 */
	void (* onCallRecvText )( CallHandler CallId, const char * pTxt );

	/** \brief Outgoing call was redirected
	 *
	 *  The outgoing call was redirected before it was answered.  Currently
	 *  only SIP can have call redirection.
	 *
	 *  After this callback the call will be terminated.  It is up to the
	 *  user to decide if they should call the number they are being
	 *  redirected to.
	 *
	 *  \param[in] CallId The call that is being redirected
	 *  \param[in] pPeer The display name of the target we're being redirected
	 *             to (named by our current peer)
	 *  \param[in] pPeerNumber The number we're being redirected to.  Can be
	 *             used with WrapperContext::CallCreate() to create a new call.
	 *  \param[in] pPeeerURI The full URI we're being redirected to.
	 *
	 *  \sa WrapperContext::CallCreate(), onCallCreated()
	 */
	void (* onCallRedirected )( CallHandler CallId, const char * pPeer, const char * pPeerNumber, const char * pPeerURI );

	/** \brief Icoming call replacing an existing call
	 *
	 *  A new incoming call is replacing an existing call.  This is part of the
	 *  call transfer process for SIP.  The \p OldCallId stops being valid after
	 *  this, replaced by the \p NewCallId.  The new peer's information is provided
	 *  in \p pPeerName, \p pPeerNumber and \p pPeerURI.
	 *
	 *  \param[in] lRes Reserved, ignore it for now
	 *  \param[in] OldCallId Call being replaced
	 *  \param[in] NewCallId New call replacing the old one
	 *  \param[in] pPeerName New call's peer display name
	 *  \param[in] pPeerNumber New call's user/number
	 *  \param[in] pPeerURI New call's full protocol-dependant URI
	 *
	 *  \sa onCallCreated(), onCallRedirected()
	 */
	void (* onCallReplace )( LIBRESULT lRes, CallHandler OldCallId, CallHandler NewCallId,
	                         const char * pPeerName, const char * pPeerNumber, const char * pPeerURI );
    
	/** \brief Call was removed from the conference
	 *
	 *  Confirmation that the call has left the conference after
	 *  WrapperContext::LeaveCallFromConference() was called.
	 *
	 *  \param[in] ConferenceId The conference
	 *  \param[in] CallId The call that has left it
	 *
	 *  \sa WrapperContext::LeaveCallFromConference()
	 */
	void (* onCallLeftConference )( ConferenceHandler ConferenceId, CallHandler CallId );

	/** @} */

	/** \addtogroup audiocbk Audio and sound callbacks
	 *
	 *  @{
	 */

	/** \brief The recording has finished
	 *
	 *  The entire sample buffer is full and we had stopped recording.
	 *
	 *  \param[in] RecordingId Recording that has finished
	 *
	 *  \sa onPlaybackFinished(), WrapperContext::AddRecording(),
	 *      WrapperContext::StartRecording(), WrapperContext::StopRecording(),
	 *      WrapperContext::GetRecordingBuffer()
	 */
	void (* onRecordingFinished )( RecordingHandler RecordingId );

	/** \brief The playback has finished
	 *
	 *  The sound has been played out.
	 *
	 *  \param[in] SoundId The sound that has finished
	 *
	 *  \sa onRecordingFinished(), WrapperContext::StartPlayback(),
	 *      WrapperContext::StopPlayback()
	 */
	void (* onPlaybackFinished )( SoundHandler SoundId );

	/** @} */

	/** \addtogroup stuncbk STUN callbacks
	 *  @{
	 */

	/** \brief Network type has been discovered
	 *
	 *  The library has discovered the type of network between this computer and
	 *  the configured STUN server.  Each STUN server can pass through different
	 *  networks depending on the configuration.
	 *
	 *  \param[in] StunId The STUN that was used to discover the network type
	 *  \param[in] netType The network type discovered
	 *
	 *  STUN is usable for SIP and RTP only for netwroks of types E_NETWORK_FULLCONE_NAT,
	 *  E_NETWORK_PORTRESTRICTED_NAT and E_NETWORK_RESTRICTEDCONE_NAT.
	 *
	 *  If the discovery has failed with result type E_NETWORK_BLOCKED, the STUN
	 *  server should be left active to retry discovery.
	 *
	 *  \sa onStunPortReady(), WrapperContext::AddStunServer(),
	 *      WrapperContext::StartStunResolve(), WrapperContext::StopStunResolve(),
	 *      WrapperContext::GetStunResolvedAddress(), WrapperContext::GetStunNetworkType()
	 */
	void (* onStunNetworkDiscovered )( StunHandler StunId, eNetworkTypeEnum_t netType );

	/** \brief Port resolving for RTP has finished
	 *
	 *  The port resolving has finished and the result is put into \p lRes.  If
	 *  successful the port is kept in a queue until an outgoing call is created
	 *  from the same user.  If the resolving has failed an outgoing call can still
	 *  be created but it will not use STUN.
	 *
	 *  \param[in] UserId The user whose STUN configuration was used to resolve the port
	 *  \param[in] CallId Obsolete, ignore
	 *  \param[in] userData The user data passed to WrapperContext::PrepareStunPort()
	 *  \param[in] lRes Result of the resolving process
	 *
	 *  \sa WrapperContext::PrepareStunPort()
	 */
	void (* onStunPortReady )( UserHandler UserId, CallHandler CallId, void * userData, LIBRESULT lRes );

	/** @} */

	/** \addtogroup diagcbk Diagnostic callbacks
	 *  @{
	 */

	/** \brief Display a warning message
	 *
	 *  This is a request from the library for the GUI to display a warning
	 *  message.  This is an obsolete/debug callback that is not used in
	 *  releases.
	 *
	 *  \param[in] pMessage Message to display (UTF-8)
	 *
	 *  \sa WrapperContext::SipDebugLog()
	 */
	void (* onDebugLog )( const char * pMessage );

	/** @} */

	/** \addtogroup faxcbk Fax callbacks
	 *
	 *  @{
	 */

	/** \brief Incoming fax offer
	 *
	 *  The remote peer has sent a fax offer.  The offer should be either accepted
	 *  with WrapperContext::FaxAccept() or rejected with WrapperContext::FaxReject().
	 *
	 *  \param[in] CallId Call on which the fax offer came
	 *
	 *  \sa WrapperContext::FaxAccept(), WrapperContext::FaxReject(),
	 *      onFaxOutgoingOffer(), onFaxStarted(), onFaxRemoteId(),
	 *      onFaxPage(), onFaxError(), onFaxDone()
	 */
	void (* onFaxIncomingOffer )( CallHandler CallId );

	/** \brief Outgoing fax offer
	 *
	 *  The library has sent the outgoing fax offer (part of the process after
	 *  the call was created with WrapperContext::CallCreateFax().
	 *
	 *  \param[in] CallId Call that has sent the fax offer
	 */
	void (* onFaxOutgoingOffer )( CallHandler CallId );

	/** \brief Fax transmission started
	 *
	 *  Valid for both incoming and outgoing fax transmissions.
	 *
	 *  \param[in] CallId The call which has switched to fax mode
	 *
	 *  \sa onFaxIncomingOffer(), onFaxOutgoingOffer(),
	 *      WrapperContext::CallCreateFax()
	 */
	void (* onFaxStarted )( CallHandler CallId );

	/** \brief Received the remote fax id
	 *
	 *  Received the remote peer's CSI through the fax transmission.
	 *
	 *  \param[in] CallId The fax call
	 *  \param[in] pFaxId The remote peer's id (a telephone number)
	 *
	 *  \sa onFaxStarted(), onFaxPage(), WrapperContext::SetUserFaxId()
	 */
	void (* onFaxRemoteId )( CallHandler CallId, const char * pFaxId );

	/** \brief Started transmitting the next page
	 *
	 *  \param[in] CallId Fax call that has a new page
	 *
	 *  \sa onFaxStarted(), onFaxRemoteId(), onFaxDone(),
	 *      WrapperContext::CallCreateFax()
	 */
	void (* onFaxPage )( CallHandler CallId );

	/** \brief Fatal error with the fax stream
	 *
	 *  This usually means the fax was either incomplete or entirely
	 *  unsuccesful.
	 *
	 *  \param[in] CallId Fax call with error
	 *  \param[in] causeCode Code of the error
	 *
	 *  \sa onFaxDone()
	 */
	void (* onFaxError )( CallHandler CallId, int causeCode );

	/** \brief Fax transmission has ended
	 *
	 *  Does not mean the fax transmission was successful
	 *
	 *  \param[in] CallId Fax call that has ended
	 *
	 *  \sa onFaxError(), WrapperContext::CallCreateFax()
	 */
	void (* onFaxDone )( CallHandler CallId );

	/** @} */

	/** \addtogroup imgcbk Image conversion callbacks
	 *
	 *  @{
	 */

	/** \brief Image conversion process
	 *
	 *  Reports the image conversion process.
	 *
	 *  \param[in] ImageId Image being processed
	 *  \param[in] TotalProgress Total progress expected (virtual units)
	 *  \param[in] CurrentProgress Current progress (same units as \p TotalProgress)
	 *
	 *  \sa onFaxImgLoaded(), WrapperContext::FaxImageLoad()
	 */
	void (* onFaxImgProcess )( ImageHandler ImageId, DWORD TotalProgress, DWORD CurrentProgress );

	/** \brief Image conversion finished
	 *
	 *  The image has been processed.  If \p Status is L_OK the file was valid
	 *  though there could be zero pages available.
	 *
	 *  The library will count all lines processed and the lines with errors. It is up
	 *  to the library user to decide how many errors are too much for the file to be
	 *  rejected.
	 *
	 *  Things to check:
	 *  \li If \p Status is not L_OK, the image is not good
	 *  \li If \p PageCount is 0, there is nothing to fax
	 *  \li If \p ErrorLines are above certain percent of \p TotalLines, a warning
	 *      should be displayed to the user and/or the image should be discarded.
	 *
	 *  \param[in] ImageId The image that was processed
	 *  \param[in] Status L_OK on success
	 *  \param[in] PageCount Number of pages processed
	 *  \param[in] TotalLines Total number of lines across all pages
	 *  \param[in] ErrorLines Number of lines with errors
	 *
	 *  \sa onFaxImgProcess(), WrapperContext::FaxImageLoad(),
	 *      WrapperContext::FaxImageDestroy(), WrapperContext::CallCreateFax()
	 */
	void (* onFaxImgLoaded )( ImageHandler ImageId, LIBRESULT Status, int PageCount, int TotalLines, int ErrorLines );

	/** @} */

	/** \addtogroup blfcbk Busy Lamp Field Callbacks
	 *
	 *  @{
	 */

	/** \brief New dialog added for the peer
	 *
	 *  The peer has created a new dialog (the peer is making or receiving a
	 *  call).  A onPeerDialogChanged() will be sent when the call enters a
	 *  new state.
	 *
	 *  \param[in] PeerId The peer that has a new dialog
	 *  \param[in] DialogId The new dialog's id
	 *  \param[in] pDialogIdStr The new dialog's tag
	 *  \param[in] NewCount The updated count of dialogs for this user
	 *
	 *  \sa onPeerDialogChanged(), onPeerDialogRemoved(),
	 *      WrapperContext::AddPeer(), WrapperContext::RemovePeer()
	 */
	void (* onPeerDialogAdded )( PeerHandler PeerId, DialogHandler DialogId, const char * pDialogIdStr, int NewCount );

	/** \brief Dialog state changed
	 *
	 *  \param[in] DialogId The dialog that has entered a new state
	 *  \param[in] NewState The new state
	 *
	 *  \sa onPeerDialogAdded(), onPeerDialogRemoved(),
	 *      WrapperContext::AddPeer(), WrapperContext::RemovePeer()
	 */
	void (* onPeerDialogChanged )( DialogHandler DialogId, ePeerState_t NewState );

	/** \brief Dialog removed
	 *
	 *  \param[in] PeerId The peer that has a dialog removed
	 *  \param[in] DialogId The dialog that has been removed
	 *  \param[in] NewCount The peer's updated dialog count after the removal
	 *
	 *  \sa onPeerDialogAdded(), onPeerDialogChanged(),
	 *      WrapperContext::AddPeer(), WrapperContext::RemovePeer()
	 */
	void (* onPeerDialogRemoved )( PeerHandler, DialogHandler, int NewCount);

	/** @} */

	/** \addtogroup videocbk Video callbacks
	 *
	 *  @{
	 */

	/** \brief Video threads started
	 *
	 *  The video processing threads have been started.  The call is ready to
	 *  encode and send frames over the network.  Use the WrapperContext::VideoSendFrame()
	 *  function to send frames.
	 *
	 *  \param[in] CallId The video call
	 *  \param[in] pThreadId Thread id for WrapperContext::VideoSendFrame()
	 *  \param[in] codec The negotiated video codec
	 *
	 *  \sa onVideoStopped(), onVideoFormatSelected(), onVideoOffered(),
	 *      WrapperContext::VideoSendFrame(), WrapperContext::CallCreateVideo()
	 *      WrapperContext::SetVideoFrameIYUVCbk(), WrapperContext::CallDisableVideo()
	 */
	void (* onVideoStarted )( CallHandler CallId, void * pThreadId, CodecEnum_t codec );

	/** \brief Video threads stopped
	 *
	 *  The library has stopped the video threads.  WrapperContext::VideoSendFrame() \em MUST
	 *  be called with \p pBuffer set to 0 to release the allocated resources.
	 *
	 *  \param[in] CallId The call that has the video stopped
	 *  \param[in] pThreadId The id of the thread that was stopped
	 *
	 *  \sa onVideoStarted(), WrapperContext::VideoSendFrame()
	 */
	void (* onVideoStopped )( CallHandler CallId, void * pThreadId );

	/** \brief Video format selected
	 *
	 *  \param[in] CallId The call for which the video format was selected
	 *  \param[in] dir The direction
	 *  \param[in] width Width in pixels
	 *  \param[in] height Height in pixels
	 *  \param[in] fps Frames per seconds
	 *
	 *  \sa WrapperContext::SetVideoFrameIYUVCbk(), onVideoStopped()
	 */
	void (* onVideoFormatSelected )( CallHandler CallId, eCallDirection_t dir, int width, int height, float fps );

	// Video was offered by the remote party during a call (can happen only when the call is in Up state)
	
	/** \brief Video offered for audio calls
	 *
	 *  The remote party has offered us video during a normal (audio) call.
	 *  Use WrapperContext::CallEnableVideo() or WrapperContext::CallDisableVideo()
	 *  to either accept or reject it.
	 *
	 *  \param[in] CallId The call
	 *
	 *  \sa WrapperContext::CallEnableVideo(), WrapperContext::CallDisableVideo()
	 */
	void (* onVideoOffered )( CallHandler CallId );

	/** @} */

	/** \addtogroup prescbk Presence callbacks
	 *
	 *  @{
	 */

	/** \brief Subscription request received
	 *
	 *  Use WrapperContext::SubscriptionReject() or WrapperContext::SubscriptionAccept()
	 *  to either reject or accept the subscription request.
	 *
	 *  \param[in] UserId The user for which we received a subscription request
	 *  \param[in] SubId The newly created subscriber id that will handle this subscription
	 *  \param[in] Phone Phone/username of the subscriber
	 *  \param[in] Name Display name (full name) of the subscriber
	 *  \param[in] Uri URI (protocol specific identification) of the subscriber
	 *
	 *  \sa WrapperContext::SubscriptionReject(), WrapperContext::SubscriptionAccept(),
	 *      onSubscriptionTerminated(), onContactStatus(), onContactTerminated()
	 */
	void (* onSubscriptionRequest )( UserHandler UserId, SubscriberHandler SubId,
	                                 const char * Phone, const char * Name, const char * Uri );

	/** \brief Subscription has been terminated
	 *
	 *  This will not be called for subscriptions on which WrapperContext::SubscriptionReject()
	 *  was used.  This means the remote peer has unsubscribed for our presence.
	 *
	 *  \param[in] UserId The user for which we had an accepted subscriber
	 *  \param[in] SubId The subscriber that is ending the subscription
	 *  \param[in] reason Reserved for future use; ignore
	 *
	 *  \sa onSubscriptionRequest(), WrapperContext::SubscriptionReject(),
	 *      WrapperContext::SubscriptionAccept()
	 */
	void (* onSubscriptionTerminated )( UserHandler UserId, SubscriberHandler SubId, const char * reason );

	/** \brief Contact status update
	 *
	 *  The contact has accepted our request and is sending its status.
	 *
	 *  \param[in] UserId The user for which the contact was added
	 *  \param[in] ContactId The contact
	 *  \param[in] ContactState The contact status (online/offline)
	 *  \param[in] pNote Note (human readable, optional)
	 *
	 *  \sa WrapperContext::AddContact(), WrapperContext::RemoveContact(),
	 *      WrapperContext::RefreshContact(), onContactTerminated(),
	 *      onContactRetrying()
	 */
	void (* onContactStatus )( UserHandler UserId, ContactHandler ContactId,
	                           eContactState_t ContactState, const char * pNote );

	/** \brief The contact has stopped sending its status
	 *
	 *  The \p ContactId becomes invalid after this callback.  Depending on
	 *  the rejection type passed through \p rejType we can re-subscribe for
	 *  this contact.
	 *
	 *  \param[in] UserId The user for which the contact was added
	 *  \param[in] ContactId The contact
	 *  \param[in] rejType Termination type: rejected (hard rejection), deactivated (soft, going offline etc)
	 *  \param[in] pReason Reserved; ignore
	 *
	 *  \sa onContactStatus(), onContactRetrying(), WrapperContext::RefreshContact(),
	 *      WrapperContext::AddContact(), WrapperContext::RemoveContact()
	 */
	void (* onContactTerminated )( UserHandler UserId, ContactHandler ContactId,
	                               eRejectionType_t rejType, const char * pReason );

	/** \brief Temporary error occured on this contact
	 *
	 *  The status of this contact is currently unknown because of a network
	 *  error.  The library will automatically retry subscribing for this
	 *  contact on its own (and deliver a onContactStatus() event when this
	 *  happens).
	 *
	 *  \param[in] UserId The user for which the contact was added
	 *  \param[in] ContactId The contact
	 *
	 *  \sa onContactStatus(), onContactTerminated(), WrapperContext::RefreshContact(),
	 *      WrapperContext::AddContact(), WrapperContext::RemoveContact()
	 */
	void (* onContactRetrying )( UserHandler UserId, ContactHandler ContactId );

	/** \brief User's status published
	 *
	 *  \param[in] UserId The user
	 *
	 *  \sa onPublicationRetrying(), onPublicationFailed(),
	 *      WrapperContext::PublishStatus(), WrapperContext::CancelPublication()
	 */
	void (* onPublicationSucceeded )( UserHandler UserId );

	/** \brief User's status publication temporary error
	 *
	 *  We were unable to publish our status but we're going to retry.
	 *
	 *  \param[in] UserId The user
	 *
	 *  \sa onPublicationSucceeded(), onPublicationFailed(),
	 *      WrapperContext::PublishStatus(), WrapperContext::CancelPublication()
	 */
	void (* onPublicationRetrying )( UserHandler UserId );

	/** \brief User's status publication has failed
	 *
	 *  The publication attempt failed with a fatal error (for example the
	 *  server does not support this feature at all).  Publication will
	 *  not be retried until WrapperContext::PublishStatus() is called again.
	 *
	 *  \param[in] UserId The user
	 *
	 *  \sa onPublicationSucceeded(), onPublicationRetrying(),
	 *      WrapperContext::PublishStatus(), WrapperContext::CancelPublication()
	 */
	void (* onPublicationFailed )( UserHandler UserId );

	/** \brief Instant message received
	 *
	 *  The message can be addressed to a known local user in which case
	 *  the \p UserId parameter will be filled with its user handler or
	 *  in case the destination was unknown it will be INVALID_HANDLE.
	 *
	 *  This can either come from a known contact (\p ContactId will not be
	 *  INVALID_HANDLE) or from an unknown contact.  In both cases the
	 *  contact name and number will be filled.  The message body can be
	 *  in any format, specified in the \p contentType MIME type string.
	 *  The library will always acknowledge the messages even if the
	 *  content is unparsable.
	 *
	 *  \param[in] UserId The user for which the message was received (can be INVALID_HANDLE if no match)
	 *  \param[in] ContactId The contact that is sending the message or INVALID_HANDLE if no match
	 *  \param[in] contactPhone The contact's phone (key identifier/sip username) (always set)
	 *  \param[in] contactName The contact's "display" name (might be empty/0)
	 *  \param[in] contentType The message's body's content type (text/plain, text/html, etc.)
	 *  \param[in] contentLength The message's body size in bytes
	 *  \param[in] body The message's body (not 0-terminated!)
	 *
	 *  \sa onContactIsComposing(), WrapperContext::SendPlainMessage(),
	 *      WrapperCotnext::SetMessageComposingState(), WrapperContext::AddContact()
	 */
	void (* onMessageReceived )( UserHandler UserId, ContactHandler ContactId,
	                             const char * contactPhone, const char * contactName,
	                             const char * contentType, int contentLength, const char * body );

	/** \brief The instant message was successfully sent
	 *
	 *  \param[in] UserId The user that sent the message
	 *  \param[in] ContactId The contact that received the message
	 *  \param[in] MessageId The message that was successfully sent
	 *  \param[in] contentLength The message's body size in bytes
	 *  \param[in] body The message's body (same as was sent with WrapperContext::SendPlainMessage())
	 *
	 *  \sa onMessageFailed(), WrapperContext::SendPlainMessage(),
	 *      WrapperCotnext::SetMessageComposingState()
	 */
	void (* onMessageSent )( UserHandler UserId, ContactHandler ContactId,
	                         MessageHandler MessageId, int contentLength, const char * body );

	/** \brief The sending of an instant message has failed
	 *
	 *  \param[in] UserId The user that failed to send the message
	 *  \param[in] ContactId The contact that failed to receive the message
	 *  \param[in] MessageId The message that failed to be sent
	 *  \param[in] contentLength The message's body size in bytes
	 *  \param[in] body The message's body (same as was sent with WrapperContext::SendPlainMessage())
	 *  \param[in] releaseCause error code
	 *
	 *  \sa onMessageSent(), WrapperContext::SendPlainMessage()
	 */
	void (* onMessageFailed )( UserHandler UserId, ContactHandler ContactId,
	                           MessageHandler MessageId, int contentLength,
	                           const char * body, int releaseCause );

	/** \brief Contact composing notification
	 *
	 *  A contact's composing status has changed (typing notification on/off)
	 *
	 *  \param[in] UserId The user for which the message is received (can be INVALID_HANDLE if no match)
	 *  \param[in] ContactId The contact that is composing the message or INVALID_HANDLE if no match
	 *  \param[in] contactPhone The contact's phone (key identifier/sip username) (always set)
	 *  \param[in] contactName The contact's "display" name (might be empty or 0)
	 *  \param[in] active 1 if the contact is composing, 0 if the contact is not composing
	 *  \param[in] contentType The content type of the message the contact is composing (can be empty or 0)
	 *  \param[in] lastActiveDT If active==0, this can contain the dateTime (xml dateTime) when the contact was last active (can be empty or 0)
	 *
	 *  \sa onMessageReceived, WrapperContext::SetMessageComposingState(),
	 *      WrapperContext::SendPlainMessage()
	 */
	void (* onContactIsComposing )( UserHandler UserId, ContactHandler ContactId,
	                                const char * contactPhone, const char * contactName,
	                                int active, const char * contentType, const char * lastActiveDT );

	/** @} */

	/** \brief Fatal error in the library
	 *
	 *  \ingroup diagcbk
	 *
	 *  Fatal error occured in the library (unhandled error condition).
	 *
	 *  \param[in] errsrc Error source
	 *  \param[in] msg Message body
	 *  \param[in] causeCode Detailed error code (if detailed reporting is on)
	 *
	 *  \sa onDebugLog()
	 */
	void (* onGeneralFailure )( ErrorSources_t errsrc, const char * msg, int causeCode );

	/** \brief Call energy levels update
	 *
	 *  \ingroup audiocbk
	 *
	 *  \param[in] CallId Call for which the update came, or INVALID_HANDLE
	 *  \param[in] inlevel Input energy level in dBm0
	 *  \param[in] outlevel Output energy level in dBm0
	 *
	 *  \sa onAudioInputLevelChange(), onAudioOutputLevelChange()
	 */
	void (* onCallAudioLevels )( CallHandler CallId, double inlevel, double outlevel );

	/** \brief Input volume level changed
	 *
	 *  \ingroup audiocbk
	 *
	 *  This callback is fired when the audio subsystem detects a change in
	 *  the input volume settings caused by external factors (for example
	 *  the user changing the input device volume from the system mixer).
	 *
	 *  The rate of this callback is controlled by the delta setting configured
	 *  by WrapperContext::SetAudioDeltaLevel().
	 *
	 *  \param[in] devId The device id
	 *  \param[in] inlevel Input level from 0.0 (silent) to 1.0 (full volume)
	 *
	 *  \sa WrapperContxet::SetAudioDeltaLevel(), onAudioOutputLevelChange(),
	 *      onAudioRingingLevelChange(), WrapperContext::SetAudioInputLevel(),
	 *      WrapperContext::GetAudioInputLevel()
	 */
	void (* onAudioInputLevelChange )( int devId, double inlevel );

	/** \brief Output volume level changed
	 *
	 *  \ingroup audiocbk
	 *
	 *  This callback is fired when the audio subsystem detects a change in
	 *  the output volume settings caused by external factors (for example
	 *  the user changing the output device volume from the system mixer).
	 *
	 *  The rate of this callback is controlled by the delta setting configured
	 *  by WrapperContext::SetAudioDeltaLevel().
	 *
	 *  \param[in] devId The device id
	 *  \param[in] outlevel Output level from 0.0 (silent) to 1.0 (full volume)
	 *
	 *  \sa WrapperContxet::SetAudioDeltaLevel(), onAudioInputLevelChange(),
	 *      onAudioRingingLevelChange(), WrapperContext::SetAudioOutputLevel(),
	 *      WrapperContext::GetAudioOutputLevel()
	 */
	void (* onAudioOutputLevelChange )( int devId, double outlevel );

	/** \deprecated Not used
	 */
	void (* onAudioMicBoostChanged )( int devId, int enabled );

	/** \brief Received an update about the remote peer identification
	 *
	 *  \ingroup callcbk
	 *
	 *  Peer information has been updated.
	 *
	 *  \param[in] CallId The call
	 *  \param[in] pPeer New peer name (display name)
	 *  \param[in] pPeerNumber New peer number (user name)
	 *  \param[in] pPeerURI New peer URI
	 *
	 *  \sa onCallCreated()
	 */
	void (* onCallPeerInfo )( CallHandler CallId, const char * pPeer, const char * pPeerNumber, const char * pPeerURI );

	/** \brief Cisco report for server forwarding
	 *
	 *  \ingroup usercbk
	 *
	 *  Cisco report for server forwarding, optionally for a specified line.
	 *
	 *  Can receive more than one update per user depending on the
	 *  number of lines configured on the server.
	 *
	 *  \param[in] userId The user
	 *  \param[in] lineNumber The line number (1 or above), or 0 if not specified
	 *  \param[in] pFwdNumber The number to which calls are being forwarded or empty if no forwarding is on
	 *  \param[in] toVoiceMail 1 if the forwarding is to the voice mail (only if \p pFwdNumber is set)
	 */
	void (* onCiscoServerFwd )( UserHandler userId, unsigned int lineNumber, const char * pFwdNumber, int toVoiceMail );

	/** \brief Reserved
	 *
	 * Reserved
	 */
	void (* onReserved001 )();

	/** \brief Reserved
	 *
	 * Reserved
	 */
	void (* onReserved002 )();

	/** \brief Reserved
	 *
	 * Reserved
	 */
	void (* onReserved003 )();


	/** \brief Sip header dump for a call
	 *
	 *  \ingroup callcbk
	 *
	 *  Dumps the header of a SIP message from a SIP call.
	 *  For incoming calls, this is the header of the INVITE message.
	 *  For outgoing calls, this is the header of the 200 response.
	 *
	 *  Each header field from the SIP header is represented as an entry
	 *  in \p pHeaderFields . The end of the header fields is marked with
	 *  a NULL \p name and \p values .
	 *
	 *  Each header field can have one or more values associated with it
	 *  in the \p values array. The end of the values is marked with a
	 *  NULL pointer.
	 *
	 *  The header field \p name and the \p values are 0-terminated
	 *  UTF-8 strings.
	 *
	 *  The structure is valid only for the duration of this callback.
	 *
	 *  To enable this callback, use WrapperContext::SipUserHeaderDump() .
	 *
	 *  \param[in] CallId The call
	 *  \param[in] pHeaderFields The header field array
	 *
	 *  \sa WrapperContext::SipUserHeaderDump(),
	 *      WrapperContext::SipUserAddHeader(),
	 *      WrapperContext::SipUserClearHeader()
	 */
	void (* onSipCallHeaderFields )( CallHandler CallId, const sSipHeaderField_t * pHeaderFields );

	/** \brief Secure Certificate Error
	 *
	 *  \ingroup diagcbk
	 *
	 *  Reports a certificate error (currently only for SIP).
	 *
	 *  \param[in] errorMask A mask of errors for the certificate, see eSecureCertError_t
	 *  \param[in] pCertSubject The certificate's subject
	 *  \param[in] pCertIssuer The certificate's issuer
	 *  \param[in] pCertNotBefore The certificate's start of validity
	 *  \param[in] pCertNotAfter The certificate's end of validity
	 *  \param[in] pExpectedName The expected subject name for the certificate (NOTE 1)
	 *  \param[in] pActualNameList The actual certificate names list (NOTE 1)
	 *  \param[in] pCertDataPEM The certificate itself, PEM encoded
	 *  \param[in] certDataLen The length in bytes of the data at \p pCertDataPEM
	 *
	 *  NOTE 1: The main certificate name is in the certificate's subject in the
	 *          'commonName' field. More names can be found in the certificate's
	 *          'altSubjectName' extension. The whole list is in the actual list
	 *          \p pActualNameList represented as a ASCIIZ string of comma delimited
	 *          values. These two parameters can be empty if the check has not reached
	 *          that stage.
	 */
	void (* onSecureCertError )( int errorMask, const char * pCertSubject, const char * pCertIssuer,
	                             const char * pCertNotBefore, const char * pCertNotAfter,
	                             const char * pExpectedName, const char * pActualNameList,
	                             const char * pCertDataPEM, int certDataLen );

	/** \brief Sip header dump for a user registration
	 *
	 *  \ingroup usercbk
	 *
	 *  Dumps the header of a SIP message from a SIP registration.
	 *  This is the header of the 200 response to the REGISTER request.
	 *
	 *  Each header field from the SIP header is represented as an entry
	 *  in \p pHeaderFields . The end of the header fields is marked with
	 *  a NULL \p name and \p values .
	 *
	 *  Each header field can have one or more values associated with it
	 *  in the \p values array. The end of the values is marked with a
	 *  NULL pointer.
	 *
	 *  The header field \p name and the \p values are 0-terminated
	 *  UTF-8 strings.
	 *
	 *  The structure is valid only for the duration of this callback.
	 *
	 *  To enable this callback, use WrapperContext::SipUserHeaderDump() .
	 *
	 *  \param[in] UserId The user
	 *  \param[in] pHeaderFields The header field array
	 *
	 *  \sa WrapperContext::SipUserHeaderDump(),
	 *      WrapperContext::SipUserAddHeader(),
	 *      WrapperContext::SipUserClearHeader(),
	 *      WrapperCallbacks::onSipCallHeaderFields()
	 */
	void (* onSipUserHeaderFields )( UserHandler UserId, const sSipHeaderField_t * pHeaderFields );

	/** \brief Full list of remote's codec offer, when available
	 *
	 *  \ingroup callcbk
	 *
	 *  Provides the full list of the remote end's codec offer,
	 *  if a list is available. For outgoing calls there is big
	 *  chance the list will contain only one codec, especially
	 *  for IAX2 calls.
	 *
	 *  The \p pCodecList parameter is a simple int array of
	 *  integers, each being a cast from CodecEnum_t. The
	 *  last element in the array is always CODEC_UNKNOWN.
	 *
	 *  The array is not "nul terminated" as commonly done in
	 *  C because the zero value is CODEC_PCMU :)
	 *
	 *  The \p hasG729 parameter will be set to 1 if the list
	 *  includes CODEC_G729, otherwise it will be set to 0.
	 *  This is mainly to make the check that this callback was
	 *  made for easier :)
	 *
	 *  \param[in] CallId The call
	 *  \param[in] hasG729 1 if the list includes CODEC_G729,
	 *                     0 otherwise
	 *  \param[in] pCodecList List of remote codecs ending
	 *                        with CODEC_UNKNOWN
	 */
	void (* onCallRemoteCodecList )( CallHandler CallId, int hasG729, const int * pCodecList );

	/** \addtogroup probecbk Probe callbacks
	 *
	 *  @{
	 */

	/** \brief User settings probe error
	 *
	 *  An error occured during user probing. The process might
	 *  continue with a different protocol.
	 *
	 *  \param[in] ProbeId The probe id
	 *  \param[in] curState The state that generated the error.
	 *  \param[in] causeCode Code of the error
	 *
	 *  \sa WrapperContext::ProbeSipTransport(), WrapperCallbacks::onProbeState(),
	 *      WrapperCallbacks::onProbeSuccess(), WrapperCallbacks::onProbeFailed()
	 */
	void (* onProbeError )( ProbeHandler ProbeId, eProbeState_t curState, int causeCode );

	/** \brief User settings probe state update
	 *
	 *  The probing process has entered another stage
	 *
	 *  \param[in] ProbeId The probe id
	 *  \param[in] ProbeState The new state
	 *
	 *  \sa WrapperContext::ProbeSipTransport(), WrapperCallbacks::onProbeError(),
	 *      WrapperCallbacks::onProbeSuccess(), 
	 */
	void (* onProbeState )( ProbeHandler ProbeId, eProbeState_t newState );

	/** \brief User settings probe success
	 *
	 *  The probing was successful. The process has ended.
	 *
	 *  \param[in] ProbeId The probe id
	 *  \param[in] Trans The recommended transport
	 *
	 *  \sa WrapperContext::ProbeSipTransport(), WrapperCallbacks::onProbeState(),
	 *      WrapperCallbacks::onProbeError(), WrapperCallbacks::onProbeFailed()
	 */
	void (* onProbeSuccess )( ProbeHandler ProbeId, eUserTransport_t Trans );

	/** \brief User settings probe aborted
	 *
	 *  The probing process was unsuccesful. The process has ended.
	 *
	 *  \param[in] ProbeId The probe id
	 *  \param[in] CauseCode The final error code
	 *
	 *  \sa WrapperContext::ProbeSipTransport(), WrapperCallbacks::onProbeState(),
	 *      WrapperCallbacks::onProbeError(), WrapperCallbacks::onProbeSuccess()
	 */
	void (* onProbeFailed )( ProbeHandler ProbeId, int CauseCode );

	/** @} */

	/** \brief Activation completed
	 *
	 *  \ingroup callbacks
	 *
	 *  Activation process result.
	 *
	 *  This callback will be invoked if and only if the activation process
	 *  has completed unambiguously. This means that we have reached the server
	 *  successfully and got a proper result, be it successful or not.
	 *
	 *  This means that in case of an error, it is a final error and not a
	 *  temporary error. No timeouts or other temporary errors will result in
	 *  this callback. Temporary errors will lead to the library retrying
	 *  gracefully until a unambiguous result is obtained.
	 *
	 *  The activation process will retry until it either gets a proper
	 *  response from the server or it is cancelled by
	 *  WrapperContext::StopActivation().
	 *
	 *  The result from the server can be either "ok" followed by a decrypted
	 *  certificate or "error" followed by an error string and optionally by
	 *  a encrypted certificate.
	 *
	 *  In case the result from the server was "ok", the certificate was
	 *  decrypted and then the result parsed successfully
	 *    - status will be E_ACT_SUCCESS
	 *    - certificate will contain the decrypted certificate
	 *    - build, hddSerial, mac and checksum will be the decrypted contents
	 *      of the certificate, pre-parsed for easier processing. They can
	 *      also be found in the certificate blob in their original form.
	 *
	 *  In case the result from the server was "ok" but we could not parse
	 *  the response AFTER the certificate was successfully decrypted:
	 *    - status will be E_ACT_SUCCESS_UNPARSABLE
	 *    - certificate will contain the decrypted certificate
	 *    - build, hddSerial, mac and checksum will be populated depending
	 *      on how far the parser could process the response
	 *
	 *  In case the result from the server was "ok" but we could not decrypt
	 *  the certificate:
	 *    - status will be E_ACT_FAILED_DECRYPT
	 *    - certificate will contian the encrypted certificate as received
	 *      from the server
	 *    - build, hddSerial, mac and checksum will be empty strings
	 *
	 *  In case the result from the server was "ok" and the certificate was
	 *  decrypted successfully but the executable checksum did not match:
	 *    - status will be E_ACT_FAILED_CHECKSUM
	 *    - certificate will contain the decrypted certificate
	 *    - build, hddSerial, mac and checksum will be the parsed values
	 *      from the certificate.
   *
	 *  In case the result from the server was "ok" and the certificate was
	 *  decrypted successfully but the neither the external hdd serial or 
   *  the mac fileds did not match:
	 *    - status will be E_ACT_FAILED_ID
	 *    - certificate will contain the decrypted certificate
	 *    - build, hddSerial, mac and checksum will be the parsed values
	 *      from the certificate.
	 *
	 *  In case the result from the server was "error":
	 *    - status will be E_ACT_FAILED
	 *    - reason will contain the error text from the server response
	 *      (the portion starting at the second line until the end of the body)
	 *    - certificate, build, hddSerial, mac and checksum will be empty strings
	 *
	 *  \sa WrapperContext::StartActivation(), WrapperContext::StopActivation(),
	 *      WrapperContext::RSADecryptData()
	 */
	void (* onActivationCompleted )( eActivationStatus_t status, const char * reason,
	                                 const char * certificate, const char * build,
	                                 const char * hddSerial, const char * mac,
	                                 const char * checksum );


	/** \brief Library requests external audio
	 *
	 *  \ingroup audiocbk
	 *
	 *  Only for external audio builds.
	 *
	 *  The library is requesting the external audio to be started.
	 *  Calls to WrapperContext::ExternalAudioFrame() are expected to start
	 *  coming at the configured sampling rate by WrapperContext::ExternalAudioInit()
	 */
	void (* onExternalAudioRequested )( void );

	/** \addtogroup zrtpcbk ZRTP callbacks
	 *
	 *  @{
	 */

	/** \brief ZRTP negotiation failed for a call
	 *
	 *  The ZRTP negotiation for a call has failed. More detailed error information
	 *  can be obtained from the CauseCode value. Enable detailed errors to see
	 *  the protocol-level strings.
	 *
	 *  ZRTP can be retried for the same call with WrapperContext::CallEnableZrtp()
	 *
	 *  \param[in] CallId The call ID
	 *  \param[in] CauseCode The error code
	 *
	 *  \sa WrapperContext::CallEnableZrtp()
	 */
	void (* onCallZrtpFailed )( CallHandler CallId, int CauseCode );

	/** \brief ZRTP negotiation succeeded for a call
	 *
	 *  The ZRTP negotiation for a call has succeeded. This does not always mean full
	 *  security yet, due to the nature of ZRTP. Even though at this stage there is
	 *  an active SRTP encryption, the keys used might be compromised.
	 *
	 *  To make sure the keys were safe the participants will need to do verbal
	 *  comparison of a short string derived from the same calculations.
	 *  In case the identify of the participants was confirmed by using secret
	 *  data from previous calls SAS verification is not required.
	 *
	 *  If the peer in this call was known (found in the cache) the \p KnownPeer
	 *  parameter will be set to 1. If we meet this peer for the first time, the
	 *  parameter will be set to 0.
	 *
	 *  If the peer knows us, the \p PeerKnowsUs parameter will be set to 1. If
	 *  the peer sees us for the first time, or they see some other problem with
	 *  our identity, this flag will be set to 0.
	 *
	 *  Note that this information comes over the encrypted channel protected by ZRTP
	 *  but there is a small chance the encryption was compromised (i.e. do not
	 *  completely trust this flag).
	 *
	 *  If the peer is known and we agree on the retained secrets in our caches
	 *  (both us and them), the CacheMismatch will be set to 0.
	 *
	 *  If the peer was known but we disagree on the retained secret there might
	 *  be a security problem or one of us might have a corrupted cache file. In
	 *  this case the CacheMismatch will be set to 1.
	 *
	 *  If the peer is not known, the \p CacheMismatch value is set to 1 to make it
	 *  easier to check if we need SAS comparison by only looking at one parameter
	 *  instead of making complex logic checks.
	 *
	 *  If the peer sent us an indication that they do not know us from previous
	 *  calls (\p PeerKnowsUs set to 0), \p CacheMismatch will also be set to 1 to
	 *  force SAS verification.
	 *
	 *  The string that needs to be compared is called the Short Authentication String.
	 *  It is returned in the \p Sas parameter. Although \p Sas is based on a binary
	 *  hash, it is encoded in a human-readable form. The encoding type is returned
	 *  in the \p SasEncoding paramter. Check WrapperContext::AddUserZrtpSasEncoding()
	 *  for more information about the supported SAS encodings.
	 *
	 *  The ZRTP RFC recommends using the following warning text if SAS comparison
	 *  is required for a known peer (i.e. when \p KnownPeer is 1 but \p CacheMismatch
	 *  is also set to 1):
	 *
	 *  Long version:
	 *      We expected the other party to have a shared secret cached from a
	 *      previous call, but they don't have it.  This may mean your peer
	 *      simply lost their cache of shared secrets, but it could also mean
	 *      someone is trying to wiretap you.  To resolve this question you
	 *      must check the authentication string with your peer.  If it
	 *      doesn't match, it indicates the presence of a wiretapper.
	 *
	 *  Short version:
	 *      Something's wrong.  You must check the authentication string with
	 *      your peer.  If it doesn't match, it indicates the presence of a
	 *      wiretapper.
	 *
	 *  Even if \p KnownPeer is 1 and \p CacheMismatch is 0 and \p PeerKnowsUs is 1,
	 *  the SAS should be made available on demand from the user. If the user demands
	 *  SAS verification and it fails, WrapperContext::CallConfirmZrtpSas() should
	 *  be called with \p Confirmed set to 0. The call will be treated as insecure
	 *  and the user will be alerted.
	 *
	 *  More information about the ZRTP negotiation is returned in the rest of the
	 *  parameters.
	 *
	 *  \p Cipher and \p AuthTag are of interest as they'll be employed by the SRTP
	 *  encryption and it might be useful to display them somewhere.
	 *
	 *  \p Hash and \p KeyAgreement are the hash algorithm and the key agreement method
	 *  used in this call.
	 *
	 *  At any time after this callback but still during the lifetime of this call
	 *  the library controller can use WrapperContext::CallConfirmZrtpSas() to either
	 *  confirm or reject the SAS.
	 *
	 *  \param[in] CallId The call ID
	 *  \param[in] PeerZid The peer's ZRTP ID (ZID) in HEX representation
	 *  \param[in] KnownPeer 1 if the peer is known, 0 if we see them for the first time
	 *  \param[in] CacheMismatch 1 if the peer needs to be confirmed, 0 if the cache agrees
	 *  \param[in] PeerKnowsUs 1 if the peer knows us, 0 if they see us for the first time or
	 *                         if they have encountered a cache mismatch of their own
	 *  \param[in] SasEncoding The SAS encoding
	 *  \param[in] Sas The Short Authentication String to be confirmed if needed. The
	 *                 string is in human readable form in the encoding \p SasEncoding
	 *  \param[in] Hash The chosen Hash Algorithm
	 *  \param[in] Cipher The chosen Cipher Algorithm (used in SRTP)
	 *  \param[in] AuthTag The chosen Authentication Tag type (used in SRTP)
	 *  \param[in] KeyAgreement The key agreement method used for the negotiation
	 *
	 *  \sa WrapperContext::CallConfirmZrtpSas()
	 */
	void (* onCallZrtpSuccess )( CallHandler CallId, const char * ZidHex, int KnownPeer,
	                             int CacheMismatch, int PeerKnowsUs, eZRTPSASEncoding_t SasEncoding,
	                             const char * Sas, eZRTPHashAlgorithm_t Hash,
	                             eZRTPCipherAlgorithm_t Cipher, eZRTPAuthTag_t AuthTag,
	                             eZRTPKeyAgreement_t KeyAgreement );

	/** @} */

	/** \brief Call channel security level has changed
	 *
	 *  \ingroup callcbk
	 *
	 *  The security level for a call can change through its lifetime depending on the
	 *  availability of ZRTP and SDES SRTP.
	 *
	 *  This callback is also invoked even if no ZRTP and no SDES SRTP is configured.
	 *
	 *  Normal unencrypted calls will have the level E_CALL_SECURITY_NONE. Expect this
	 *  callback to be invoked in the beginning of such calls. This will happen also
	 *  for calls that have ZRTP enabled because in this case the call will also start
	 *  as a normal unencrypted call.
	 *
	 *  For SDES SRTP calls, the security level depends on the security level of the
	 *  SIP channel that was used to exchange the SRTP master keys. If the SIP channel
	 *  was secure, the call will also be secure. Those calls will get an immediate
	 *  callback with the level set to E_CALL_SECURITY_SDES_SRTP.
	 *
	 *  For any call (be it unencrypted or protected by SDES RTP and SIP/TLS) a ZRTP
	 *  negotiation might be requested at any moment. When the ZRTP negotiation
	 *  completes successfully, the SRTP encryption will start using a new set of
	 *  keys. If the peer was unknown or if there was a cache mismatch, this callback
	 *  will be invoked with the level set to E_CALL_SECURITY_ZRTP_UNCOFRIMED. At
	 *  this stage the users are expected to confirm the SAS and indicate final
	 *  success or failure. The call security level will progress either to
	 *  E_CALL_SECURITY_ZRTP_SRTP (if the SAS is confirmed) or to E_CALL_SECURITY_NONE
	 *  or E_CALL_SECURITY_SDES_SRTP (if the SAS is rejected).
	 *
	 *  Last but not least, there can be separate media channels for the audio and the
	 *  video in a call. You might receive more than one call security level callback
	 *  depending on which channel is being secured. For ZRTP securing of the audio
	 *  channel happens before securing of the video channel. In fact, the video channel
	 *  ZRTP negotiation depends on the outcome of the audio channel ZRTP negotiation.
	 *
	 *  Color-coding the security level can be used to represent the security level
	 *  in a visual way. Using a padlock icon could also help represent the security
	 *  situation to the user more naturally. The following scheme can be used:
	 *
	 *  E_CALL_SECURITY_NONE              Red, Unlocked
	 *  E_CALL_SECURITY_ZRTP_UNCOFRIMED   Orange/Yellow, Locked
	 *  E_CALL_SECURITY_ZRTP_SRTP         Green, Locked
	 *  E_CALL_SECURITY_SDES_SRTP         Green or Blue, Locked
	 *
	 *  Note that a confirmed ZRTP negotiation is considered more secure than a
	 *  SDES SRTP exchange. This can be underscored by using a different color coding
	 *  for each.
	 *
	 *  \param[in] CallId The call Id
	 *  \param[in] CallChannel The channel type
	 *  \param[in] SecurityLevel The call security level
	 */
	void (* onCallSecurityLevel )( CallHandler CallId, eCallChannel_t CallChannel, eCallSecurityLevel_t SecurityLevel );

	/** \brief Secondary stream failed to negotiate ZRTP 
	 *
	 *  \ingroup zrtpcbk
	 *
	 *  Multistream ZRTP negotiation has failed for a secondary stream in a call.
	 *
	 *  ZRTP requires only the first RTP stream in a SIP call to be negotiated with
	 *  a full Diffie-Hellman key exchange.
	 *
	 *  Due to the nature of ZRTP the primary RTP stream is an audio stream (to allow
	 *  for voice confirmation of the SAS).
	 *
	 *  Subsequent streams in a call, like the video stream for example, will use
	 *  a shorter version of ZRTP called "Multistream ZRTP" to negotiate the keys.
	 *
	 *  In case a SIP call is configured with both ZRTP and Video, after the ZRTP
	 *  finishes securing the audio channel, the SIP call will automatically try to
	 *  secure the video channel. If this secondary negotiation fails this callback will
	 *  be fired.
	 *
	 *  This error does NOT mean that the primary channel is suddenly broken. There
	 *  are no side effects from this failure. The remote end might have a different
	 *  opinion on the matter and might decide to close the call.
	 *
	 *  \param[in] CallId The call Id on which the secondary ZRTP negotiation failed
	 *  \param[in] CallChannel The type of the secondary RTP stream (usually video)
	 *  \param[in] CauseCode Detailed error information about the failure
	 */
	void (* onCallZrtpSecondaryError )( CallHandler CallId, eCallChannel_t CallChannel, int CauseCode );

	/** \brief Call Network Quality Level changed
	 *
	 *  \ingroup callcbk
	 *
	 *  onCallNetworkQualityLevel() is fired when the quality status for the network
	 *  changes. The network quality is determined by measurements on the incoming traffic
	 *  generated by a call.
	 *
	 *  The network quality measurements are done end-to-end. In some configurations
	 *  a bad network quality might mean problem with the service provider. In others
	 *  this might mean problems with the local Wi-Fi. The library does not have the
	 *  tools to diagnose each network node between the local machine and the machine
	 *  on the other end of the call. It must be up to the end user to interpret the
	 *  quality measurements.
	 *
	 *  In the beginning of each call onCallNetworkQualityLevel() will be invoked with
	 *  status E_NET_QUALITY_PENDING. This might happen at other times and means that
	 *  the network quality is going to be re-evaluated. In 5 to 10 seconds the result
	 *  of the evaluation will be sent by a second invocation of onCallNetworkQualityLevel().
	 *  Afterwards the network will be constantly evaluated, but unless the status
	 *  changes there will be no more onCallNetworkQualityLevel() callbacks.
	 *
	 *  The quality level is represented by the eNetworkQualityLevel_t enum. Possible visual
	 *  encodings are given in eNetworkQualityLevel_t's definition. It is left to the GUI
	 *  to decide how to represent the levels. Suggested possibilities are: 1 to 3 stars,
	 *  color-coded (for a LED-like visual), indicator bars similar to the wireless signal
	 *  strength found on mobile phones; etc.
	 *
	 *  Note that this is not a measurement on the call quality itself, as it can be
	 *  only measured by the humans doing the calls.
	 *
	 *  \param[in] CallID The call ID for which the quality level is being updated
	 *  \param[in] CallChannel The call's channel that is being updated (audio or video)
	 *  \param[in] QualityLevel The new quality level for the call channel
	 *
	 */
	void (* onCallNetworkQualityLevel )( CallHandler CallId, eCallChannel_t CallChannel,
		eNetworkQualityLevel_t QualityLevel );

	/** \brief Call Channel Network Statistics
	 *
	 *  \ingroup callcbk
	 *
	 *  onCallNetworkStatistics() will be fired periodically (current interval is 5 seconds)
	 *  to update the call network statistics for a given call channel (either the audio
	 *  stream or the video stream in a call).
	 *
	 *  For each media stream a bunch of statistics are calculated. The calculations are
	 *  based on the media payload that is being sent or received. Apart from the pure
	 *  payload, there is protocol header overhead. For example even though a 20ms GSM
	 *  packet takes 33 bytes, the network overhead of the various protocol levels can
	 *  amount to an additional 54 bytes which is more than the actual audio payload.
	 *
	 *  This means that for an RTP/UDP packet over Ethernet or Wi-Fi, the protocol
	 *  overhead can be 21.6kbps in each direction, while the moderately compressed GSM
	 *  audio frame is doing only 13kbps in each direction.
	 *
	 *  The library does not have complete knowledge over the network protocols overhead.
	 *  It knows the size of the RTP and UDP headers for SIP, and the size of the IAX2
	 *  mini frame header for IAX2, but it can only estimate the size of the IP header
	 *  which is fully controlled by the operating system. Furthermore it cannot know
	 *  if the packets are being transmitted over Ethernet but it estimates that it is
	 *  so.
	 *
	 *  This reflects on the statistics provided by onCallNetworkStatistics(). The real
	 *  numbers are the packet counts and the payload byte counts. The total counters
	 *  are calculated by multiplying a pre-defined network overhead size with the
	 *  packet count and then adding the actual payload byte count. The bit rates are
	 *  calculated from the last N seconds from a running window over the estimated
	 *  totals. The average bit rates are calculated from the total estimates for the
	 *  entire channel lifetime.
	 *
	 *  The input packet loss and input packet arrival jitter are calculated on the
	 *  library's perception of time and its ability to process incoming packets.
	 *  There are no estimates there but these two numbers are completely subjective.
	 *
	 *  Interpretation of these values is done by the Library and results in
	 *  onCallNetworkQualityLevel() being called when a noticeable change in the
	 *  network quality is detected. The GUI should not try to interpret these values;
	 *  it should either display them as they are or ignore this callback.
	 *
	 *  \param[in] CallId The call ID for which these statistics apply
	 *  \param[in] CallChannel The channel for which these statistics apply (video or audio)
	 *  \param[in] TotalInputPackets Total number of packets received
	 *  \param[in] TotalInputBytes Total bytes received, including protocol overhead estimates
	 *  \param[in] TotalInputBytesPayload Total payload-only bytes received (no estimates)
	 *  \param[in] CurrentInputBitrate Bit rate of incoming traffic including overhead estimates,
	 *                                 for the last few seconds (in bits per second)
	 *  \param[in] AverageInputBitrate Average bit rate of the incoming traffic, including
	 *                                 overhead estimates (in bits per second)
	 *  \param[in] TotalOutputPackets Total number of packets sent
	 *  \param[in] TotalOutputBytes Total bytes sent, including protocol overhead estimates
	 *  \param[in] TotalInputBytesPayload Total payload-only bytes received (no estimates)
	 *  \param[in] CurrentOutputBitrate Bit rate of outgoing traffic including overhead estimates,
	 *                                  for the last few seconds (in bits per second)
	 *  \param[in] AverageOutputBitrate Average bit rate of the outgoing traffic, including
	 *                                  overhead estimates (in bits per second)
	 *  \param[in] CurrentInputLossPermil Current incoming loss in permil (0 to 1000)
	 *  \param[in] CurrentInputJitterMs Current incoming jitter in milliseconds
	 */
	void (* onCallNetworkStatistics )( CallHandler CallId, eCallChannel_t CallChannel,
		unsigned long TotalInputPackets, unsigned long TotalInputBytes, unsigned long TotalInputBytesPayload,
		unsigned long CurrentInputBitrate, unsigned long AverageInputBitrate,
		unsigned long TotalOutputPackets, unsigned long TotalOutputBytes, unsigned long TotalOutputBytesPayload,
		unsigned long CurrentOutputBitrate, unsigned long AverageOutputBitrate,
		int CurrentInputLossPermil, int CurrentInputJitterMs );

	/** \brief Ringing volume level change
	 *
	 *  \ingroup audiocbk
	 *
	 *  This callback is fired when the audio subsystem detects a change in
	 *  the ringing volume settings caused by external factors (for example
	 *  the user changing the ringing device volume from the system mixer).
	 *
	 *  The rate of this callback is controlled by the delta setting configured
	 *  by WrapperContext::SetAudioDeltaLevel().
	 *
	 *  \param[in] devId The device id
	 *  \param[in] ringlevel Ringing level from 0.0 (silent) to 1.0 (full volume)
	 *
	 *  \sa WrapperContxet::SetAudioDeltaLevel(), onAudioInputLevelChange(),
	 *      onAudioOutputLevelChange(), WrapperContext::SetAudioRingingLevel(),
	 *      WrapperContext::GetAudioRingingLevel()
	 */
	void (* onAudioRingLevelChange )( int devId, double ringlevel );

	/** \addtogroup pushcbk Push notification callbacks
	 *
	 *  @{
	 */

	/** \brief Push token received by the service
	 *
	 *  The push token has been successfully sent to the service and it has sent
	 *  a confirmation.
	 *
	 *  In case the \p deviceId in WrapperContext::PublishPushToken() was empty or
	 *  NULL the Library has generated and used a random one which is returned
	 *  here.
	 *
	 *  \param[in] serviceUserId The service
	 *  \param[in] deviceId The device ID
	 *
	 *  \sa WrapperContext::PublishPushToken(), onPushTokenFailure()
	 */
	void (* onPushTokenSuccess )( UserHandler serviceUserId, const char * deviceId );

	/** \brief Error while trying to send the push token
	 *
	 *  There was an error while communicating with the service. Another attempt will
	 *  be automatically done after \p willRetryAfterSeconds seconds unless there
	 *  was a fatal error.
	 *
	 *  Most errors are treated as temporary and the Library will continue retrying
	 *  until it succeeds. In rare occasions where the error is really fatal and
	 *  WRAPPER can't continue \p willRetryAfterSeconds will be set to -1.
	 *
	 *  Fatal errors usually happen when the service account does not support the
	 *  push notification extensions.
	 *
	 *  \param[in] serviceUserId Service
	 *  \param[in] causeCode Error code or detailed error code
	 *  \param[in] willRetryAfterSeconds Seconds after which WRAPPER will
	 *                                   automatically retry or -1 if it won't
	 *
	 *  \sa WrapperContext::PublishPushToken(), onPushTokenSuccess()
	 */
	void (* onPushTokenFailure )( UserHandler serviceUserId, int causeCode, int willRetryAfterSeconds );

	/** \brief Provider specific token was successfully configured
	 *
	 *  \param[in] serviceUserId Service user
	 *  \param[in] providerUserId Provider user
	 *  \param[in] providerId Local provider id
	 *  \param[in] providerToken Public provider token (generated by the service and
	 *                           given to the provider)
	 *
	 *  \sa WrapperContext::RequestProviderToken(), onProviderTokenFailure(),
	 *      onProviderTokenRemoval()
	 */
	void (* onProviderTokenSuccess )( UserHandler serviceUserId, UserHandler providerUserId,
	                                  const char * providerId, const char * providerToken );

	/** \brief Error while negotiating provider specific token
	 *
	 *  There are many stages of the token negotiation that can fail. In most
	 *  cases WRAPPER will automatically retry and this event is informational.
	 *
	 *  \p willRetryAfterSeconds will be set to a number greater than zero to indicate
	 *  that the error was non-fatal and that WRAPPER will automatically retry.
	 *  If \p willRetryAfterSeconds is set to -1 then provider token for this provider
	 *  cannot be negotiated and WRAPPER will not retry.
	 *
	 *  \param[in] serviceUserId Service user
	 *  \param[in] providerUserId Provider user
	 *  \param[in] causeCode Error code or detailed error code
	 *  \param[in] willRetryAfterSeconds Automatic retry in seconds or -1 for fatal errors
	 *
	 *  \sa WrapperContext::RequestProviderToken(), onProviderTokenSuccess(),
	 *      onProviderTokenFailure(), onProviderTokenRemoval()
	 */
	void (* onProviderTokenFailure )( UserHandler serviceUserId, UserHandler providerUserId,
	                                  int causeCode, int willRetryAfterSeconds );

	/** \brief Provider specific token was successfully removed
	 *
	 *  This callback is invoked when WrapperContext::RemoveProviderToken() completes
	 *  its task.
	 *
	 *  \param[in] serviceUserId Service user
	 *  \param[in] providerUserId Provider user
	 *
	 *  \sa WrapperContext::RemoveProviderToken(), WrapperContext::RequestProviderToken(),
	 *      onProviderTokenSuccess(), onProviderTokenFailure()
	 */
	void (* onProviderTokenRemoval )( UserHandler serviceUserId, UserHandler providerUserId );

	/** @} */

	/** \brief Asynchronous WAV load completed
	 *
	 *  \ingroup soundcbk
	 *
	 *  Invoked when the asynchronous load of WrapperContext::AddSoundFromWav()
	 *  has been completed.
	 *
	 *  \param[in] soundId The sound that has completed
	 *  \param[in] result The completion status, L_OK if ok, L_FAIL on error
	 *  \param[in] causeCode In case result is L_FAIL, the error code
	 */
	void (* onSoundLoadCompleted )( SoundHandler soundId, LIBRESULT result, int causeCode );
	
	
	/** \brief Latency test has completed
	 * 
	 *  Invoked when the latency test started by WrapperContext::StartLatencyTest()
	 *  has been completed. 
	 * 
	 *	\param[in] status Status of the test. L_OK if test completed without 
	 *			error. L_FAIL if not able to open audio streams, usually due to
	 *			unsupported sample rate and/or buffer size and/or stereo/mono mode
	 *			L_NO_MEM if no memory could be allocated for analysis. In that
	 *			case, try decreasing the testing time. 
	 * \param[in] latency1 Latency in ms with the largest correlation value
	 * \param[in] latency2 Latency in ms with the smallest correlation value. If
	 *			the measurement is correct should usually be within a few ms from 
	 *			latency 1. If the difference is larger, it usually indicates an 
	 *			incorrect test, for example due to too noisy test environment, 
	 *			bad speaker/microphone or incorrect device volume settings. 
	 * \param[in] maxRecordInputLevel Maximum level seen in the recording buffer.
	 *			A value of less than ~2000 indicates that the signal is too quiet
	 *			and the results could be unreliable. A value of more than 32000
	 *			indicates that the input signal was probably too loud and some 
	 *			clipping has occured
	 * 	 
	 * \sa WrapperContext::StartLatencyTest()
	 */
	void (* onLatencyTestCompleted )(LIBRESULT status, int latency1, int latency2, int maxRecordInputLevel );

	/** \brief Warning for missing SIP Outbound support at the server
	 *
	 *  \ingroup usercbk
	 *
	 *  Happens when there is a SIP account with SIP Outbound enabled but the
	 *  server rejects the registrations with the error code 439, usually with
	 *  the text "First Hop Lacks Outbound Support".
	 *
	 *  The full error information is passed in the registration error failure
	 *  callback as usual. This event is pushed immediately after the registration
	 *  failure event.
	 *
	 *  The library will NOT alter its configuration automatically but if this
	 *  behavior is desired the API user can just do the needed reconfiguration
	 *  in this callback and issue a new WrapperContext::RegisterUser() request
	 *  afterwards for immediate retry.
	 *
	 *  \param[in] userId The user which got the registration error
	 */
	void (* onUserSipOutboundMissing )( UserHandler userId );

	/** \brief Call hold request has completed
	 *
	 *  \ingroup callcbk
	 *
	 *  Invoked when an outgoing (local) call hold request has completed. This
	 *  does not always mean an actual protocol-level hold request was sent.
	 *
	 *  For example in an early SIP call WrapperContext::CallHold() currently
	 *  has no protocol-level action (no UPDATE/PRACK support yet) but this
	 *  callback will still arrive (almost immediately).
	 *
	 *  In some cases the hold request might be rejected by the remote peer.
	 *  In these cases the call is still considered "on hold". The "hold"
	 *  is simulated by muting the media. The event will arrive with remoteStatus
	 *  set to "L_FAIL".
	 *
	 *  In case there is local Music on Hold (see WrapperContext::LoadMusicServiceFile())
	 *  there is no protocol-level hold but the callback will arrive with
	 *  remoteStatus set to "L_OK".
	 *
	 *  \param[in] callId The ID of the call
	 *  \param[in] remoteStatus See above
	 *
	 *  \sa onCallUnholdCompleted(), onCallRefreshCompleted(),
	 *      WrapperContext::CallHold(), WrapperContext::CallUnhold(),
	 *      WrapperContext::CallRefresh()
	 */
	void (* onCallHoldCompleted )( CallHandler callId, LIBRESULT remoteStatus );

	/** \brief Call unhold request has completed
	 *
	 *  \ingroup callcbk
	 *
	 *  Invoked when an outgoing (local) call unhold request has completed.
	 *
	 *  See onCallHoldCompleted() for more details
	 *
	 *  \param[in] callId The ID of the call
	 *  \param[in] remoteStatus See onCallHoldCompleted()
	 *
	 *  \sa onCallHoldCompleted(), onCallRefreshCompleted(),
	 *      WrapperContext::CallHold(), WrapperContext::CallUnhold(),
	 *      WrapperContext::CallRefresh()
	 */
	void (* onCallUnholdCompleted )( CallHandler callId, LIBRESULT remoteStatus );

	/** \brief Call refresh request has completed
	 *
	 *  \ingroup callcbk
	 *
	 *  Invoked when an outgoing (local) call refresh request has completed.
	 *
	 *  See onCallHoldCompleted() for more details regarding asynchronous
	 *  execution.
	 *
	 *  This callback is the result of WrapperContext::CallRefresh(). It
	 *  currently works only for SIP.
	 *
	 *  Unlike WrapperContext::CallHold() and WrapperContext::CallUnhold(),
	 *  this always means that a SIP re-INVITE transaction has completed,
	 *  either successfully or unsuccesfully. In both cases the call is
	 *  kept up.
	 *
	 *  \param[in] callId The ID of the call
	 *  \param[in] remoteStatus Result of the re-INVITE
	 *
	 *  \sa onCallHoldCompleted(), onCallUnholdCompleted(),
	 *      WrapperContext::CallHold(), WrapperContext::CallUnhold(),
	 *      WrapperContext::CallRefresh()
	 */
	void (* onCallRefreshCompleted )( CallHandler callId, LIBRESULT remoteStatus );

	/** \brief Remote network statistics (RTCP sender report)
	 *
	 *  \ingroup callcbk
	 *
	 *  For SIP calls this is invoked when a RTCP report is received.
	 *  For IAX2 calls this callback is not invoked.
	 *
	 *  The current packet loss count is given in parts per thousand
	 *  (permil). It will be in the range from 0 to 1000. To convert
	 *  to percent divide by 10.
	 *
	 *  The packet lost counter is "total", i.e. for the entire call.
	 *
	 *  The interarrival jitter is given in milliseconds.
	 *
	 *  The round trip time is calculated from the DSLR field in the
	 *  RTCP Sender Report. Some devices send improbable DSLR timings
	 *  that result in negative round trip. These will be forced to
	 *  zero instead of returning a negative delay.
	 *
	 *  \param[in] callId The ID of the call
	 *  \param[in] currentPacketsLostPermil Current packet loss, 0 to 1000 permil
	 *  \param[in] totalPacketsLost Total packets lost for the current duration of the call
	 *  \param[in] currentInterarrivalJitterMs Interarrival jitter in milliseconds
	 *  \param[in] currentRoundtripTimeMs Current roundtrip time in milliseconds
	 *
	 *  \sa onCallNetworkStatistics()
	 */
	void (* onCallRemoteNetworkStatistics )( CallHandler callId,
		int currentPacketsLostPermil, unsigned long totalPacketsLost,
		int currentInterarrivalJitterMs, int currentRoundtripTimeMs );

	/** \brief Audio Watchdog Restart notification
	 *
	 *  \ingroup audiocbk
	 *
	 *  This notification accompanies an automatic audio restart.
	 *
	 *  It is caused by an audio driver getting stuck causing audio
	 *  frames to stop flowing.
	 *
	 *  The audioDebugInfo parameter can include an audio endpoint
	 *  name and/or direction. This could be logged or ignored. This
	 *  API is not completely finalized.
	 *
	 *  There is no need to do anything for this event. The audio
	 *  restart happens automatically (and probably has already started).
	 *
	 *  \param[in] audioDebugInfo Additional debug info
	 */
	void (* onAudioWatchdogRestart )( const char * audioDebugInfo );

	/** @} */
} WrapperCallbacks;

/** \addtogroup customcbk Custom callbacks
 *
 *  These callbacks are each individually set by functions from
 *  WrapperContext instead of being part of the WrapperCallbacks
 *  structure.
 *
 *  @{
 */

/** \addtogroup exteccbk External audio processing callbacks
 *
 *  The external echo cancellation filter callbacks are not
 *  part of the WrapperCallbacks structure because they're not
 *  invoked by WrapperContext::PollEvents().  They're called
 *  directly from the audio processing threads.
 *
 *  @{
 */

/** \brief Obsolete. Do not use.
 */
typedef int (* pfInitExtECCbk ) ( void );

/** \brief Obsolete. Do not use.
 */
typedef int (* pfGetExtECVersionCbk ) ( void );

/** \brief Obsolete. Do not use.
 */
typedef int (* pfExtECProcessSpkrBlockCbk )( const short *, size_t, short *, size_t );

/** \brief Obsolete. Do not use.
 */
typedef int (* pfExtECProcessMicBlockCbk )( const short *, size_t, short *, size_t );

/** \brief Obsolete. Do not use.
 */
typedef void (* pfCloseExtECCbk )( void );

/** @} */

/** \addtogroup videocbk Video callbacks
 *
 *  Currnetly having only one callback, the frame callback.
 *  Unlike the callbacks in the WrapperCallbacks structure this
 *  one is called directly form the video stream threads.
 *
 *  @{
 */

/** \brief Video frame is ready for displaying
 *
 *  A video frame received from the network has been successfully
 *  decoded and is ready to be displayed.  The video frame is
 *  always in aligned planar YUV 4:2:0 (YUV420P) format.  The
 *  dimensions of the frame is always provided.
 *
 *  Each line in each plane is aligned (alignment values are also
 *  provided).  The significant pixels start at the beginning of
 *  the line followed by optional padding.
 *
 *  \param[in] callId The call on which the frame was received
 *  \param[in] pMatchedThread The sibling frame thread in the
 *             same video thread (given only in case the user
 *             needs to match them)
 *  \param[in] width The width of the frame in pixels
 *  \param[in] height The height of the frame in pixels
 *  \param[in] y_plane Aligned luma plane buffer
 *  \param[in] u_plane Aligned U-chroma plane buffer
 *  \param[in] v_plane Aligned V-chroma plane buffer
 *  \param[in] y_linesize Alignment of each line in the luma buffer, in bytes
 *  \param[in] u_linesize Alignment of each line in the U-chroma buffer, in bytes
 *  \param[in] v_linesize Alignment of each line in the V-chroma buffer, in bytes
 *
 *  \sa WrapperCallbacks::onVideoStarted(), WrapperCallbacks::onVideoFormatSelected()
 *      WrapperContext::SetVideoFrameIYUVCbk()
 */
#define HAVE_VideoFrameIYUVCbk 1
typedef void (* pfVideoFrameIYUVCbk )( CallHandler callId, void * pMatchedThread, int width, int height,
									   const unsigned char * y_plane, const unsigned char * u_plane, const unsigned char * v_plane,
									   int y_linesize, int u_linesize, int v_linesize );

/** @} */

/** \brief Custom event callback function
 *
 *  This callback is used with WrapperContext::AddCustomEvent().
 *  You can add many custom events at once each pointing to a different
 *  (or the same) callback.  For this reason this callback is not
 *  in the WrapperCallbacks structure although it is called from
 *  WrapperContext::PollEvents() like all the rest of the callbacks
 *  in WrapperCallbacks.
 *
 *  \param[in] pUserData The user data pointer passed when the callback
 *             was set with WrapperContext::AddCustomEvent()
 *
 *  \sa WrapperContext::AddCustomEvent(), WrapperContext::PollEvents()
 */
#define HAVE_CustomEventCbk 1
typedef void ( *pfCustomEventCbk )( void * pUserData );

/** @} */

/** \brief Video frame is ready for displaying
 *
 *  A video frame received from the network has been successfully
 *  decoded and is ready to be displayed.  The video frame is
 *  always in aligned planar YUV 4:2:0 (YUV420P) format.  The
 *  dimensions of the frame is always provided.
 *
 *  Each line in each plane is aligned (alignment values are also
 *  provided).  The significant pixels start at the beginning of
 *  the line followed by optional padding.
 *
 *  The callback is variant of pfVideoFrameIYUVCbk, but it provide
 *  custom user data and allows to be registered per each separate per call. 
 *
 *  \param[in] callId The call on which the frame was received
 *  \param[in] pUserData The user data provided at onVideoStarted()
 *             using WrapperContext::SetVideoFrameIYUVCbk2().
 *
 *  \param[in] width The width of the frame in pixels
 *  \param[in] height The height of the frame in pixels
 *  \param[in] y_plane Aligned luma plane buffer
 *  \param[in] u_plane Aligned U-chroma plane buffer
 *  \param[in] v_plane Aligned V-chroma plane buffer
 *  \param[in] y_linesize Alignment of each line in the luma buffer, in bytes
 *  \param[in] u_linesize Alignment of each line in the U-chroma buffer, in bytes
 *  \param[in] v_linesize Alignment of each line in the V-chroma buffer, in bytes
 *
 *  \sa WrapperCallbacks::onVideoStarted(), WrapperCallbacks::onVideoFormatSelected()
 *      WrapperContext::SetVideoFrameIYUVCbk(), WrapperContext::SetVideoFrameIYUVCbk2()
 */
#define HAVE_VideoFrameIYUVCbk2 1
typedef void (* pfVideoFrameIYUVCbk2 )( CallHandler callId, void * pUserData, int width, int height,
									   const unsigned char * y_plane, const unsigned char * u_plane, const unsigned char * v_plane,
									   int y_linesize, int u_linesize, int v_linesize );

/** \brief Video raw encoded data is ready for decoding
 *
 *  A video frame encoded data received from the network  is 
 *  ready to be decoded.  The
 *  dimensions of the frame are always provided.
 *
 *  \param[in] callId The call on which the frame was received
 *  \param[in] pUserData The user data provided at onVideoStarted()
 *             using WrapperContext::SetVideoFrameRAWCbk().
 *  \param[in] width The width of the frame in pixels
 *  \param[in] height The height of the frame in pixels
 *  \param[in] data Encoded frame data buffer pointer
 *  \param[in] size Encoded frame data buffer size
 *
 *  \sa WrapperCallbacks::onVideoStarted(), WrapperCallbacks::onVideoFormatSelected()
 *      WrapperContext::SetVideoFrameIYUVCbk(), WrapperContext::SetVideoFrameRAWCbk()
 */
#define HAVE_VideoFrameRAWCbk 1
typedef void (* pfVideoFrameRAWCbk )(CallHandler callId, void * pUserData, int width, int height, const unsigned char* data, int size);

/** \brief WRAPPER requests ExternalAudio to stop synchronously
 *
 *  This callback is issued by WRAPPER to the ExternalAudio implementation
 *  to request it to stop calling ExternalAudioFrame(). This callback must
 *  not return to the caller until the ExternalAudio has made sure the
 *  ExternalAudioFrame() will not be called until a new ExternalAudio
 *  request is made by onExternalAudioRequested().
 *
 */
#define HAVE_ExternalAudioSyncStopCbk 1
typedef void (* pfExternalAudioSyncStopCbk )( void * pUserData );

/** @} */

#if( defined( __cplusplus ) )
}
#endif

#endif
