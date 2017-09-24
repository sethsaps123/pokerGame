//
//  piConnect.h
//  sudsWithBuds
//
//  Created by Seth Saperstein on 9/24/17.
//  Copyright © 2017 Seth Saperstein. All rights reserved.
//

#import <Foundation/Foundation.h>
#include <sys/socket.h>
#include <netdb.h>

@interface piConnect : NSObject

@property (strong, nonatomic) id someProperty;
    
-(void)switchon:(int)on;

@end
