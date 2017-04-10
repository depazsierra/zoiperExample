//
//  zoiperVoip.h
//  zoiperVoip
//
//  Created by Diego de Paz Sierra on 7/5/16.
//  Copyright © 2016 Depa. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface ZoiperVoip : NSObject

+ (ZoiperVoip*)sharedInstance;

- (void)registerSIPWithUser:(NSString*)user pass:(NSString*)pass server:(NSString*)server proxy:(NSString*)proxy;

- (void)callNumber:(NSString*)tel;

- (void)callHangout;

- (void)setupSIP;

- (void)activationRegister:(NSString*)user password:(NSString*)pass;

@end
