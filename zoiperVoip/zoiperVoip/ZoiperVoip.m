//
//  zoiperVoip.m
//  zoiperVoip
//
//  Created by Diego de Paz Sierra on 7/5/16.
//  Copyright © 2016 Depa. All rights reserved.
//

#import "ZoiperVoip.h"
#import "ZSDKLibControl.h"

static ZoiperVoip * sharedInstance = nil;
NSTimer*                    timerLibEvents;


@implementation ZoiperVoip

- (instancetype)init
{
    self = [super init];
    if (self) {
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(ctxRegistrationSucceeded:)
                                                     name:@"ZSDKctxDidRegistrationSucceeded" object:nil];
//        [[NSNotificationCenter defaultCenter] addObserver:self
//                                                 selector:@selector(ctxActivationStatusUpdated:)
//                                                     name:@"ZSDKctxDidActivationStatusUpdated" object:nil];
        
    }
    return self;
}

+ (ZoiperVoip*)sharedInstance {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[ZoiperVoip alloc] init];
    });
    
    return sharedInstance;
}

- (void)setupSIP {
    InitLibrary(37248, 0);
    NSLog(@"Init SETUP SIP");
    
    timerLibEvents = [NSTimer scheduledTimerWithTimeInterval:0.10 target:[ZoiperVoip sharedInstance] selector:@selector(onLibraryEventsPollTimer:) userInfo:nil repeats:YES];
}

- (void)activationRegister:(NSString*)user password:(NSString*)pass {
    gWrapperCtx.StartActivationSDK(nil, [user UTF8String] , [pass UTF8String], NULL);
}

- (void)registerSIPWithUser:(NSString*)user pass:(NSString*)pass server:(NSString*)server proxy:(NSString*)proxy {
 
    const char *cstrUser = [user cStringUsingEncoding:[NSString defaultCStringEncoding]];
    const char *cstrPassword = [pass cStringUsingEncoding:[NSString defaultCStringEncoding]];
    const char *cstrServer = [server cStringUsingEncoding:[NSString defaultCStringEncoding]];
    const char *cstrProxy = [proxy cStringUsingEncoding:[NSString defaultCStringEncoding]];
    
    // Wrapper initialization
    // First create a user
    gUserId = gWrapperCtx.AddUser(PROTO_SIP, cstrUser, cstrPassword, cstrProxy, cstrServer, "", "");
    
    // Initialize codecs
    gWrapperCtx.ClearCodecList();
    
    LIBRESULT res;
    res = gWrapperCtx.AddCodec(CODEC_OPUS_FULL);
    res = gWrapperCtx.AddCodec(CODEC_OPUS_NARROW);
    res = gWrapperCtx.AddCodec(CODEC_OPUS_WIDE);
    
    gWrapperCtx.AddCodec(CODEC_PCMU);   // Audio codecs
    //gWrapperCtx.AddCodec(CODEC_PCMU);
    gWrapperCtx.AddCodec(CODEC_GSM);
    //gWrapperCtx.AddCodec(CODEC_VP8);    // Video codecs
    gWrapperCtx.AddCodec(CODEC_H263);
    gWrapperCtx.AddCodec(CODEC_H263_PLUS);
    
    // Set DTMF parameters
    gWrapperCtx.SetUserDtmfBand( gUserId, E_DTMF_MEDIA_OUTBAND );
    
    // RTP parameters
    gWrapperCtx.SetRTPSessionName( "Zoiper" );
    gWrapperCtx.SetRTPUsername( "Zoiper" );
    
    // Set-up audio subsystem parameters
    gWrapperCtx.UseEchoCancellation(0);
    gWrapperCtx.UseAutomaticGainControl(0);
    gWrapperCtx.UseNoiseSuppression(0);
    
    // Sets up video negotiation parameters
    gWrapperCtx.ClearVideoFormats();
    gWrapperCtx.AddVideoFormat(352, 288, 15);   // Could add multiple formats if needed
    gWrapperCtx.SetVideoBitrate(256000);
    
    // finally, register the user
    gWrapperCtx.RegisterUser(gUserId);
    NSLog(@"PROBAAAAAANDDOO userID:%d", gUserId);
}


- (void)ctxRegistrationSucceeded:(NSNotification*)notification {
    //[[NSNotificationCenter defaultCenter] removeObserver:self
    //                                                selector:@selector(ctxRegistrationSucceeded:)
    //                                                name:@"ZSDKctxDidRegistrationSucceeded]" object:nil];
    NSLog(@"------ ZOIPER: La registracion fue exitosa -------");
}

- (void)callNumber:(NSString*)tel {
    const char *cstrNumber = [tel cStringUsingEncoding:[NSString defaultCStringEncoding]];
    gWrapperCtx.CallCreate(gUserId, cstrNumber, &gCallId);
}

- (void)callHangout {
    if (gbInCall)
        gWrapperCtx.CallHangup(gCallId);
}

/************************************
 *  onLibraryEventsPollTimer
 ************************************/
- (void)onLibraryEventsPollTimer:(NSTimer *)timer
{
    PollLibrary();
}

@end
