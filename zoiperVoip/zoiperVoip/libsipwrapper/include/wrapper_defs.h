#if( !defined( _WRAPPER_EXTERNAL_DEFS_INCLUDED_ ) )
#define _WRAPPER_EXTERNAL_DEFS_INCLUDED_

#ifndef _BYTE_DEFINED
typedef	unsigned char   BYTE;
#	define _BYTE_DEFINED
#endif

#ifndef _WORD_DEFINED
typedef unsigned short	WORD;
#	define _WORD_DEFINED
#endif

#ifndef _DWORD_DEFINED
typedef	unsigned long   DWORD;
#	define _DWORD_DEFINED
#endif

#ifndef _LIB_RESULT_DEFINED
typedef	unsigned long        LIBRESULT;
#	define L_OK              ((LIBRESULT)0)
#	define L_INVALIDARG      ((LIBRESULT)-1)
#	define L_FAIL            ((LIBRESULT)-2)
#	define L_NO_MEM          ((LIBRESULT)-3)
#	define L_NOTFOUND        ((LIBRESULT)-4)
#	define L_NOPERM          ((LIBRESULT)-5)
#	define L_UNSUPPORTED     ((LIBRESULT)-6)
#	define L_WAIT            ((LIBRESULT)1)
#define _LIB_RESULT_DEFINED
#endif

typedef short SAMPLE;
#define SAMPLE_MIN (-32768)
#define SAMPLE_MAX (32767)

typedef DWORD   Handler;
typedef Handler	CallHandler;
typedef Handler UserHandler;
typedef Handler	SubscriberHandler;
typedef Handler SoundHandler;
typedef Handler RecordingHandler;
typedef Handler ConferenceHandler;
typedef Handler StunHandler;
typedef Handler PeerHandler;
typedef Handler DialogHandler;
typedef Handler ImageHandler;
typedef Handler ContactHandler;
typedef Handler MessageHandler;
typedef Handler RegexHandler;
typedef Handler CertHandler;
typedef Handler ProbeHandler;

#define	INVALID_HANDLE ((Handler)-1)

#if( defined( WIN32_WRAPPER_EXPORTS ) )
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

// The bits for AddContact2's subscribeFlags argument
#define CONTACT_SUBSCRIBE_PRESENCE   1       // Event: presence (SIP)
#define CONTACT_SUBSCRIBE_BLF        2       // Event: dialog (SIP)

// Pre-defined bit sets for AddContact2
#define CONTACT_SUBSCRIBE_NONE  ((long) 0)
#define CONTACT_SUBSCRIBE_ALL   (CONTACT_SUBSCRIBE_PRESENCE|CONTACT_SUBSCRIBE_BLF)

typedef enum eContactState_tag {
	CONTACT_STATE_OFFLINE	= 0
,	CONTACT_STATE_ONLINE
,	CONTACT_STATE_UNKNOWN
} eContactState_t;

typedef enum eRejectionType_tag {
	REJECTION_REJECT		= 0
,	REJECTION_DEACTIVATED
,	REJECTION_UNKNOWN
} eRejectionType_t;

/** \brief Audio and video codecs
 */
typedef enum CodecEnum_tag {
	CODEC_PCMU             = 0          /**< G.711 mu-law */
,	CODEC_GSM                           /**< GSM */
,	CODEC_G723
,	CODEC_DVI4_8K
,	CODEC_DVI4_16K
,	CODEC_LPC
,	CODEC_PCMA                          /**< G.711 a-law */
,	CODEC_G722                          /**< G.722 */
,	CODEC_L16_2CH
,	CODEC_L16_1CH
,	CODEC_QCELP
,	CODEC_CN
,	CODEC_MPA
,	CODEC_G728
,	CODEC_DVI4_11K
,	CODEC_DVI4_22K
,	CODEC_G729                          /**< G.729 */
,	CODEC_CELB
,	CODEC_JPEG                          /**< MJPEG video */
,	CODEC_NV
,	CODEC_H261
,	CODEC_MPV
,	CODEC_MP2T
,	CODEC_H263
,	CODEC_SPEEX_NARROW                  /**< Speex @ 8000 Hz */
,	CODEC_SPEEX_WIDE                    /**< Speex @ 16000 Hz */
,	CODEC_SPEEX_ULTRA                   /**< Speex @ 32000 Hz */
,	CODEC_iLBC_30                       /**< iLBC 30 */
,	CODEC_iLBC_20                       /**< iLBC 20 */
,	CODEC_G726                          /**< G.726 32 kbps */
,	CODEC_H263_PLUS                     /**< H.263+ (H263-1998) video */
,	CODEC_VP8                           /**< VP8 video */
,	CODEC_H264                          /**< H.264 video */
,	CODEC_TEL_EVENT                     /**< RFC 2833/4733 DTMF codec */
,	CODEC_OPUS_NARROW                   /**< Opus @ 8000 Hz */
,	CODEC_OPUS_WIDE                     /**< Opus @ 16000 Hz */
,	CODEC_OPUS_SUPER                    /**< Opus @ 24000 Hz */
,	CODEC_OPUS_FULL                     /**< Opus @ 48000 Hz */
,	CODEC_AMR                           /**< AMR */
,	CODEC_AMR_WB                        /**< AMR-WB */
,	CODEC_H264_HWD                      /**< H.264 (hw accelerated) */
,	CODEC_COUNT
,	CODEC_UNKNOWN          = -1
} CodecEnum_t;

// Backward compatibility
#define AudioCodecEnum_t CodecEnum_t
#define AudioCodecEnum_tag CodecEnum_tag

typedef enum eCallDirection_tag {
	eIncommingCall			= 0
,	eOutgoingCall
,	eUnknownDirectionCall	= (BYTE)-1
} eCallDirection_t;

typedef enum eAudioDeviceType_tag {
    eAudioInputDevice   = 0
,   eAudioOutputDevice
,   eAudioInOutDevice
,   eAudioDeviceCount
,   eUnknownAudioDevice = -1 // If the device does not support neither input, nor output
} eAudioDeviceType_t;

typedef enum ErrorSources_tag {
	ERR_SRC_NETWORK_ERROR	= 0		// Network connectivity error
,	ERR_SRC_PROTOCOL_ERROR			// SIP transaction error
,	ERR_SRC_INTERNAL_ERR			// Library fatal error
,	ERR_SRC_UNKNOWN					// Unknown error type 
} ErrorSources_t;

// Enable this to test RTSP as if it was SIP
#undef SIP_IS_RTSP_HAX
#if( defined( SIP_IS_RTSP_HAX ) && defined( WRAPPER_LIB ) )
typedef enum ProtoType_tag {
	PROTO_RTSP
,	PROTO_IAX
,	PROTO_XMPP                          // This is currently not used by the library but is needed in the GUI
,	PROTO_SIP
,	PROTO_UNKNOWN = (BYTE)-1
} ProtoType_t;
#else
typedef enum ProtoType_tag {
	PROTO_SIP
,	PROTO_IAX
,	PROTO_XMPP                          // This is currently not used by the library but is needed in the GUI
,	PROTO_RTSP
,	PROTO_UNKNOWN = (BYTE)-1
} ProtoType_t;
#endif

typedef enum eNetworkTypeEnum_tag {
	E_NETWORK_BLOCKED		= 0			// Network is blocked (bad, maybe STUN server is down, STUN should be OFF)
 ,  E_NETWORK_SYMMETRIC_FW				// Symmetric Firewall (bad, STUN should be OFF and hope for the best)
 ,  E_NETWORK_OPEN						// Open network (good, but STUN must be OFF)
 ,  E_NETWORK_FULLCONE_NAT				// Full Cone NAT (good, STUN should be ON)
 ,  E_NETWORK_SYMMETRIC_NAT				// Symmetric NAT (bad, STUN should be OFF and hope for the best)
 ,  E_NETWORK_PORTRESTRICTED_NAT		// Port Restricted NAT (good, STUN should be ON)
 ,  E_NETWORK_RESTRICTEDCONE_NAT		// Restricted Cone NAT (good, STUN should be ON)
 ,  E_NETWORK_COUNT
 ,  E_NETWORK_UNKNOWN		= (BYTE)-1
} eNetworkTypeEnum_t;

// SIP transports
typedef enum eUserTransport_tag {
	E_TRANSPORT_UDP			= 0			// Default user transport
 ,	E_TRANSPORT_TCP
 ,	E_TRANSPORT_TLS
 ,	E_TRANSPORT_COUNT
 ,	E_TRANSPORT_UNKNOWN		= (BYTE)-1
} eUserTransport_t;

// SIP dialog event package states
typedef enum ePeerState_tag {
	E_PEER_TRYING			= 0				// dial out, invite sent, initial state
 ,  E_PEER_PROCEEDING						// 1xx without a tag (remote dialog id not known)
 ,  E_PEER_EARLY							// 1xx with a tag, both remote and local ids known
 ,  E_PEER_CONFIRMED						// 2xx
 ,  E_PEER_TERMINATED						// on remote or local BYE, or on any error in any stage of the process
 ,  E_PEER_UNKNOWN			= (BYTE)-1
} ePeerState_t;

// DTMF band selection
typedef enum eDtmfBand_tag {
	E_DTMF_MEDIA_INBAND		= 0				// SIP: RTP samples, IAX2: media samples (not yet supported in either)
 ,  E_DTMF_MEDIA_OUTBAND					// SIP: RTP tel-event, IAX2: dtmf packet (supported in both, DEFAULT)
 ,  E_DTMF_SIGNALLING_OUTBAND				// SIP: SIP INFO, IAX2: same as MEDIA_OUTBAND
 ,  E_DTMF_UNKNOWN			= (BYTE)-1
} eDtmfBand_t;

typedef enum eDtmfCode_tag {
  DTMF_0
, DTMF_1 
, DTMF_2  
, DTMF_3  
, DTMF_4  
, DTMF_5  
, DTMF_6  
, DTMF_7  
, DTMF_8  
, DTMF_9 
, DTMF_ASTERISK 
, DTMF_POUND 
, DTMF_A  
, DTMF_B  
, DTMF_C  
, DTMF_D  
, DTMF_BS
, DTMF_UNKNOWN
, DTMF_COUNT
} eDtmfCode_t;

// SRTP negotiation method
typedef enum eSrtpNegotiation_tag {
	E_SRTP_NONE				= 0				// No key negotiation (no SRTP)
 ,  E_SRTP_SDES								// SDES, RFC 4568
 ,  E_SRTP_MIKEY							// MIKEY, RFC 4567, RFC 3830
 ,  E_SRTP_UNKNOWN			= (BYTE)-1
} eSrtpNegotiation_t;

// Call recording methods
typedef enum eCallRecording_tag {
	E_RECORDING_MIXED		= 0				// Mix local and remote talk in one channel (mono)
,	E_RECORDING_LOCAL		= 1				// Only store local talk
,	E_RECORDING_REMOTE		= 2				// Only store remote talk
,	E_RECORDING_STEREO		= 3				// Record local talk as left channel, remote talk as right channel
,	E_RECORDING_UNKNOWN		= (BYTE)-1
} eCallRecording_t;

// Fax Writer Mode
typedef enum eFaxWriterMode_tag {
	E_FAXWRITER_TIFFG3		= 0				// Recreate a fax tiff
 ,	E_FAXWRITER_FREE						// Watermarked + branded tiff for free
 ,	E_FAXWRITER_PDF							// Create a pdf (not yet supported)
 ,	E_FAXWRITER_UNKNOWN		= (BYTE)-1
} eFaxWriterMode_t;

typedef enum eOutputDeviceEnum_tag {
	E_OUTPUT_NORMAL			= 0			// Use the normal output device
 ,	E_OUTPUT_RINGING					// Use the ringing output device
 ,	E_OUTPUT_DISABLE					// Disable output
 ,	E_OUTPUT_COUNT
 ,	E_OUTPUT_UNKNOWN		= (BYTE)-1
} eOutputDeviceEnum_t;

typedef enum AesKeyLength_tag {
	AES_LEN_128_BITS	= 0
,	AES_LEN_192_BITS
,	AES_LEN_256_BITS
,	AES_LEN_COUNT
,	AES_LEN_UNKNOWN_BITS	= (unsigned char)-1
} AesKeyLength_t;

typedef enum DigestTypeEnum_tag {
	E_DIGEST_MD5			= 0
,	E_DIGEST_SHA1
,	E_DIGEST_RMD160
,	E_DIGEST_SHA256
,	E_DIGEST_COUNT
,	E_DIGEST_UNKNOWN		= (unsigned char)-1
} DigestTypeEnum_t;

/** \brief Audio device information structure
 *
 * Used by WrapperContext::GetAudioDeviceInfo()
 */
typedef struct sAudioDeviceInfo_tag
{
	int reserved1;                      //!< Unused
	const char *name;                   //!< 0-terminated UTF-8 device name
	int reserved2;                      //!< Unused
	int maxInputChannels;               //!< 1 or more if device is capable of input, 0 otherwise
	int maxOutputChannels;              //!< 1 or more if device is capable of output, 0 otherwise
} sAudioDeviceInfo_t;

typedef enum ScxRegexTypeEnum_tag {
	E_SCXREGEX_BASIC
,	E_SCXREGEX_EXTENDED
,	E_SCXREGEX_COUNT
,	E_SCXREGEX_UNKNOWN		= (unsigned char)-1
} ScxRegexTypeEnum_t;

typedef enum eCiscoDeviceType_tag {
	E_CISCO_CIPC = 30016
} eCiscoDeviceType_t;

/** \brief SIP header field structure
 *
 * \sa WrapperCallbacks::onSipCallHeaderFields(),
 *     WrapperCallbacks::onSipUserHeaderFields()
 */
typedef struct sSipHeaderField_tag
{
	const char *  name;                 //!< 0-terminated UTF-8 header field name
	const char ** values;               //!< A 0-terminated array of 0-terminated UTF-8 header field values
} sSipHeaderField_t;

typedef enum eSecureSuite_tag {
	E_SECURE_SSLv23
,	E_SECURE_TLSv1
,	E_SECURE_UNKNOWN		= (unsigned char)-1
} eSecureSuite_t;

// THIS IS A BIT-FIELD
typedef enum eSecureCertError_tag {
	E_SCERR_NONE			= 0			// No bits set, all OK
,	E_SCERR_ISSUER			= 1			// Issuer untrusted/not found/not valid/wrong purpose
,	E_SCERR_CERT			= 2			// Peer certificate broken (wrong Signature/Public Key/Purpose)
,	E_SCERR_DATE			= 4			// Certificate not yet valid or already expired
,	E_SCERR_REVOKED			= 8			// Certificate was revoked or rejected
,	E_SCERR_INTERNAL		= 16		// Internal or other unrecognized error--ask for debug log...
,	E_SCERR_NAME			= 32		// Certificate names mismatch
} eSecureCertError_t;

typedef enum eSecureCertResult_tag {
	E_SCRES_OK				= 0			// Cert + Key usable (validity check is separate)
,	E_SCRES_INVALID_SUITE				// Invalid security suite selected
,	E_SCRES_NOT_INITIALIZED				// Library is not initialized
,	E_SCRES_FILE_ERROR					// File access error (not found or bad permission)
,	E_SCRES_UNKNOWN_FORMAT				// Unrecognized file format
,	E_SCRES_BROKEN_PEM					// Recognized as PEM but the file is broken or corrupt
,	E_SCRES_BROKEN_PKCS12				// Recognized as PKCS#12 but the file is broken or corrupt
,	E_SCRES_BAD_PASSWORD				// Was unable to decode the file with the provided password
,	E_SCRES_NO_CERT						// Has no certificate (maybe it is a key file only?)
,	E_SCRES_NO_KEY						// Has no key (maybe it is a cert file only?)
,	E_SCRES_UNUSABLE_CERT				// The suite could not load the cert
,	E_SCRES_UNUSABLE_KEY				// The suite could not load the key
,	E_SCRES_CERT_NOT_PAIRED				// The cert is not paired with the key
} eSecureCertResult_t;

typedef enum eSecureUserConfig_tag {
	E_SCUSR_CLIENT_ONLY					// Disable TLS server operation for this user; use no certificate for client TLS connections (recommended)
,	E_SCUSR_COMMON						// Use the common server TLS transport from InitTls() if available
,	E_SCUSR_DEDICATED					// Create a dedicated TLS transport for this user with a certificate from a file
,	E_SCUSR_GENERATE					// Create a dedicated TLS transport for this user and generate a self-signed certificate (not recommended)
} eSecureUserConfig_t;

typedef enum eErrorLayer_tag {
	E_LAYER_UNKNOWN = 0
,	E_LAYER_WRAPPER
,	E_LAYER_SIP_LOCAL
,	E_LAYER_SIP
,	E_LAYER_IAX_LOCAL
,	E_LAYER_IAX
,	E_LAYER_XMPP_LOCAL
,	E_LAYER_XMPP
,	E_LAYER_RTSP_LOCAL
,	E_LAYER_RTSP
,	E_LAYER_ZRTP_LOCAL
,	E_LAYER_ZRTP
,	E_LAYER_HTTP
,	E_LAYER_API_DISPATCHER
,	E_LAYER_ACTIVATION
,	E_LAYER_APPLICATION
} eErrorLayer_t;

typedef enum eObjectClass_tag {
	E_OBJ_UNKNOWN = 0
,	E_OBJ_USER
,	E_OBJ_CALL
,	E_OBJ_MSG
,	E_OBJ_PROBE
} eObjectClass_t;

typedef enum eProbeState_tag {
	E_PROBE_UNKNOWN = 0
,	E_PROBE_CONFIG                      // Configuration check
,	E_PROBE_TLS                         // TLS check
,	E_PROBE_TCP                         // TCP check
,	E_PROBE_UDP                         // UDP check
} eProbeState_t;

// Copy of NetEq's WebRtcNetEQNetworkType
// Controls a multiplier for max buffer size of the packet
// buffer in which jitter, drops and so on are handled.
// Suggested default: E_NETBUF_TCP_NORMAL
typedef enum eNetworkBufferType_tag {
	E_NETBUF_UDP_NORMAL = 0                // Normal almost-no-jitter UDP network
,	E_NETBUF_UDP_VIDEOSYNC                 // Normal slightly jittery network
,	E_NETBUF_TCP_NORMAL                    // Same as UDP_VIDEOSYNC
,	E_NETBUF_TCP_LARGE_JITTER              // Twice as much as above
,	E_NETBUF_TCP_EXTRA_LARGE_JITTER        // Five times as much as above
} eNetworkBufferType_t;

typedef enum eStreamMixType_tag {
	E_STREAM_MIX_NORMAL = 0                // Normal mixing, if stereo, expand to both channels
,	E_STREAM_MIX_LEFT                      // If stereo, mix only with the left channel
,	E_STREAM_MIX_RIGHT                     // If stereo, mix only with the right channel
} eStreamMixType_t;

typedef enum eSessionTimerMode_tag {
	E_TIMER_UAC                            // User Agent Client (Caller) should do the refreshes
,	E_TIMER_UAS                            // User Agent Server (Callee) should do the refreshes
,	E_TIMER_LOCAL                          // We should do the refreshes (changes depending on call type)
,	E_TIMER_REMOTE                         // They should do the refreshes (also changes depending on call type)
,	E_TIMER_DISABLED                       // Don't offer session expiry
} eSessionTimerMode_t;

typedef enum eActivationStatus_tag {
	E_ACT_SUCCESS                          // "ok", cert decrypted, parsed
,	E_ACT_SUCCESS_UNPARSABLE               // "ok", cert decrypted, could not parse
,	E_ACT_FAILED_DECRYPT                   // "ok", cert could not be decrypted
,	E_ACT_FAILED                           // "error"
,	E_ACT_FAILED_DEADLINE                  // deadline reached while retrying
,	E_ACT_FAILED_CHECKSUM                  // "ok", but checksums don't match
,	E_ACT_FAILED_ID                        // "ok", but hdd or the mac don't match
,	E_ACT_FAILED_CACHE                     // error loading cert from cache and HTTP fallback is unavailable
} eActivationStatus_t;

typedef enum eZRTPHashAlgorithm_tag {
	E_ZRTP_HASH_S256                       // S256 (SHA-256)
,	E_ZRTP_HASH_S384                       // S384 (SHA-384)
,	E_ZRTP_HASH_N256                       // N256 (SHA-3 256) NOT SUPPORTED YET (easy)
,	E_ZRTP_HASH_N384                       // N384 (SHA-3 384) NOT SUPPORTED YET (easy)
} eZRTPHashAlgorithm_t;

typedef enum eZRTPCipherAlgorithm_tag {
	E_ZRTP_CIPHER_AES1                     // AES1 (AES-128 in CFB mode with 128 bit feedback)
,	E_ZRTP_CIPHER_AES2                     // AES2 (AES-192 in CFB-128 mode) NOT SUPPORTED YET (complicated)
,	E_ZRTP_CIPHER_AES3                     // AES3 (AES-256 in CFB-128 mode) NOT SUPPORTED YET (complicated)
,	E_ZRTP_CIPHER_2FS1                     // 2FS1 (TwoFish-128) NOT SUPPORTED YET (complicated+)
,	E_ZRTP_CIPHER_2FS2                     // 2FS2 (TwoFish-192) NOT SUPPORTED YET (complicated+)
,	E_ZRTP_CIPHER_2FS3                     // 2FS3 (TwoFish-256) NOT SUPPORTED YET (complicated+)
} eZRTPCipherAlgorithm_t;

typedef enum eZRTPAuthTag_tag {
	E_ZRTP_AUTH_HS32                       // HS32 (HMAC-SHA1 with 32-bit tag)
,	E_ZRTP_AUTH_HS80                       // HS80 (HMAC-SHA1 with 80-bit tag)
,	E_ZRTP_AUTH_SK32                       // SK32 (Skein-512-Mac with 32-bit tag) NOT SUPPORTED (complicated+)
,	E_ZRTP_AUTH_SK64                       // SK64 (Skein-512-Mac with 64-bit tag) NOT SUPPORTED (complicated+)
} eZRTPAuthTag_t;

typedef enum eZRTPKeyAgreement_tag {
	E_ZRTP_KEY_DH3K                        // DH3k (Finite-Field Diffie-Hellman with 3072-bit prime)
,	E_ZRTP_KEY_DH2K                        // DH2k (Finite-Field Diffie-Hellman with 2048-bit prime)
,	E_ZRTP_KEY_EC25                        // EC25 (Elliptic Curve Diffie-Hellman with 256-bit prime)
,	E_ZRTP_KEY_EC38                        // EC38 (Elliptic Curve Diffie-Hellman with 384-bit prime)
,	E_ZRTP_KEY_PRSH                        // PRSH (Preshared) NOT SUPPORTED (medium)
,	E_ZRTP_KEY_MULT                        // MULT (Multistream) AUTOMATIC! DO NOT CONFIGURE! NOT SUPPORTED (medium)
} eZRTPKeyAgreement_t;

typedef enum eZRTPSASEncoding_tag {
	E_ZRTP_SAS_B32                         // B32 (Base-32 Short Authentication String encoding)
,	E_ZRTP_SAS_B256                        // B256 (Base-256 (PGP Wordlist) Short Authentication String encoding)
} eZRTPSASEncoding_t;

typedef enum eCallChannel_tag {
	E_CHANNEL_AUDIO
,	E_CHANNEL_VIDEO
} eCallChannel_t;

typedef enum eCallSecurityLevel_tag {
	E_CALL_SECURITY_NONE                   // The call has no security at all (red)
,	E_CALL_SECURITY_ZRTP_UNCOFRIMED        // ZRTP was used to establish an encrypted channel but
	                                       // we need to confirm the SAS (orange)
,	E_CALL_SECURITY_ZRTP_SRTP              // The call is protected by SRTP which was
	                                       // configured by ZRTP and confirmed by the user (green)
,	E_CALL_SECURITY_SDES_SRTP              // The call is protected by SRTP which was
	                                       // configured by SDES and depends on the security level
	                                       // of the SIP/TLS channel (blue)
} eCallSecurityLevel_t;

typedef enum eNetworkQualityLevel_tag {
	E_NET_QUALITY_PENDING                 // Gray; 0/3 stars (grayed?): qualification is pending
,	E_NET_QUALITY_NONE                    // Black or Gray; 0/3 stars: No incoming packets at all
,	E_NET_QUALITY_VERY_BAD                // Red; 1/3 stars: heavy incoming packet loss or jitter > 100ms
,	E_NET_QUALITY_BAD                     // Orange or Yellow; 2/3 stars: incoming packet loss or jitter > 50ms
,	E_NET_QUALITY_NORMAL                  // Green; 3/3 stars: very low incoming packet loss or jitter > 20ms
,	E_NET_QUALITY_PERFECT                 // Green or Blue; 3/3 shiny stars: no incoming packet loss, jitter < 10ms
} eNetworkQualityLevel_t;

typedef enum eAudioDriverEngine_tag {
	E_AUDIO_DRV_PA                        // Use portaudio
,	E_AUDIO_DRV_EXTERNAL                  // Use ExternalAudio API in wrapper.h
,	E_AUDIO_DRV_VIRTUAL                   // Use virtual (silent) driver
,	E_AUDIO_DRV_DEFAULT                   // No preference, use default (most likely PA)
,	E_AUDIO_DRV_NO_CHANGE                 // Do not change the driver
} eAudioDriverEngine_t;

typedef enum eAudioDriverRate_tag {
	E_AUDIO_DRV_RATE_NO_CHANGE  = 0       // Do not change the sampling rate
,	E_AUDIO_DRV_RATE_DEFAULT    = -1      // Let the driver select the best rate
,	E_AUDIO_DRV_RATE_8000       = 8000    // Use 8000 Hz
,	E_AUDIO_DRV_RATE_16000      = 16000   // Use 16000 Hz
,	E_AUDIO_DRV_RATE_22050      = 22050   // Use 22050 Hz
,	E_AUDIO_DRV_RATE_44100      = 44100   // Use 44100 Hz
,	E_AUDIO_DRV_RATE_48000      = 48000   // Use 48000 Hz
                                          // NOTE: Selection is not limited to these values.
} eAudioDriverRate_t;

typedef enum eAudioDriverBuffer_tag {
	E_AUDIO_DRV_BUFFER_NO_CHANGE = 0      // Do not change the buffer size
,	E_AUDIO_DRV_BUFFER_DEFAULT   = -1     // Use automatic settings for the buffer size
                                          // NOTE: Buffer selection is not limited to these values
} eAudioDriverBuffer_t;

typedef enum eAudioResampler_tag {
	E_AUDIO_DRV_RESAMPLER_DEFAULT         // No preference, use default configuration
,	E_AUDIO_DRV_RESAMPLER_INTERNAL        // Use internal resampler implementation (resampler_internal.cpp) - good quality, high CPU usage
,	E_AUDIO_DRV_RESAMPLER_IPHONE          // Use iOS resampler
,	E_AUDIO_DRV_RESAMPLER_SPEEX           // Use Speex library resampler - good quality, medium CPU usage
,	E_AUDIO_DRV_RESAMPLER_WEBRTC          // Use WebRTC resamplers - low quality, low CPU usage
} eAudioResampler_t;

// THIS IS A BIT-FIELD
typedef enum eActivationFlags_tag {
	E_ACTIVATION_FLAG_NONE                           = 0
,	E_ACTIVATION_FLAG_CHECK_CACHE_ONLY               = 1    // This flag is used when we want to check the cached certificate without requesting one upon failure 
,	E_ACTIVATION_FLAG_SKIP_CHECKSUM_VERIFICATION     = 2     // Do not require checksum verification  
} eActivationFlags_t;

typedef enum eVideoFrameFormat_tag {
	E_VIDEO_FRAME_YUV420p               // YUV420 planar
,	E_VIDEO_FRAME_RGBA                  // RGBA non-planar
,	E_VIDEO_FRAME_ARGB                  // ARGB non-planar
} eVideoFrameFormat_t;

typedef enum ePushTokenFormat_tag {
	E_PUSH_WPN                         = 0       // Windows Push Notifications
,	E_PUSH_APN                         = 1       // Apple Push Notification
,	E_PUSH_GCM                         = 2       // Google Cloud Messaging
} ePushTokenFormat_t;

// THIS IS A BIT-FIELD
typedef enum eCodecFlags_tag {
	E_CODEC_HAS_AUDIO                   = 0x1     // Supports audio
,	E_CODEC_HAS_VIDEO                   = 0x2     // Supports video
,	E_CODEC_HAS_VBR_SUPPORT             = 0x4     // Supports VBR (Variable Bit Rate)
,	E_CODEC_HAS_DTX_SUPPORT             = 0x8     // Supports DTX (Discontinuous Transmission) (TX = transmission)
} eCodecFlags_t;

#endif
