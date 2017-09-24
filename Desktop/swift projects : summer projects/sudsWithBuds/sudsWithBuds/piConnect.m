//
//  piConnect.m
//  sudsWithBuds
//
//  Created by Seth Saperstein on 9/24/17.
//  Copyright © 2017 Seth Saperstein. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "piConnect.h"


@implementation piConnect

-(void)switchon:(int)on {
    int mainsock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    struct hostent *host = gethostbyname("35.0.58.28"); //set host of our server
    memcpy (&server_address.sin_addr.s_addr, host->h_addr, host->h_length); //set the host and length
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons (50007); //set port of our server
    
    if (connect (mainsock, (struct sockaddr *) &server_address, sizeof (server_address)) < 0) {
        NSLog(@"error connecting");
    }
    
    NSString *returnval = @"off";
    if (on == 1) {
        returnval = @"on";
    }
    long w = write(mainsock,[returnval UTF8String],strlen([returnval UTF8String]));
    if (on == 3) {
        write(mainsock,[@"end" UTF8String],strlen([returnval UTF8String]));
    }
    if (w < 0) {
        NSLog(@"error connecting");
    }
    close(mainsock);
}

@end
