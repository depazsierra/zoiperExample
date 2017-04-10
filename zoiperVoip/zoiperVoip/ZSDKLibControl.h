//
//  ZSDKLibControl.h
//  zoiper-sdk-demo
//
//  Created by Ivan Vasilev on 29/1/14.
//  Copyright (c) 2014 Securax Ltd. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "wrapper_defs.h"
#import "wrapper.h"

@class ZSDKLibControl;

extern WrapperContext gWrapperCtx;
extern int  gUserId;
extern BOOL gbRegistrationOk;
extern BOOL gbInCall;
extern BOOL gbActivated;
extern CallHandler gCallId;

extern void * gVideoThreadId;

void InitLibrary(int SIPPort, int IAXPort);
void PollLibrary();
