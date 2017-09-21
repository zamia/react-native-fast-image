//
//  FLAnimatedImage.h
//  Flipboard
//
//  Created by Raphael Schaad on 7/8/13.
//  Copyright (c) 2013-2015 Flipboard. All rights reserved.
//


#import <UIKit/UIKit.h>

// Allow user classes conveniently just importing one header.
#import "FLAnimatedImageView.h"

#if defined(DEBUG) && DEBUG
@protocol FLAnimatedImageDebugDelegate;
#endif


// Logging
// If set to 0, disables integration with CocoaLumberjack Logger (only matters if CocoaLumberjack is installed).
#ifndef FLLumberjackIntegrationEnabled
    #define FLLumberjackIntegrationEnabled 1
#endif

// If set to 1, enables NSLog logging (only matters #if DEBUG -- never for release builds).
#ifndef FLDebugLoggingEnabled
    #define FLDebugLoggingEnabled 0
#endif


#ifndef NS_DESIGNATED_INITIALIZER
    #if __has_attribute(objc_designated_initializer)
        #define NS_DESIGNATED_INITIALIZER __attribute((objc_designated_initializer))
    #else
        #define NS_DESIGNATED_INITIALIZER
    #endif
#endif


typedef NS_ENUM(NSUInteger, FLAnimatedImageInitMode) {
    FLAnimatedImageInitModeDefault = 0, // Streaming (fast init)
    FLAnimatedImageInitModeLazy = 1,    // Streaming Lazy (fastest init, no properties)
    FLAnimatedImageInitModeFull = 2     // For WKInterfaceImage? (slower init, uses more memory)
};


//#warning Update Description
//
//  An `FLAnimatedImage`'s job is to deliver frames in a highly performant way and works in conjunction with `FLAnimatedImageView`.
//  It subclasses `NSObject` and not `UIImage` because it's only an "image" in the sense that a sea lion is a lion.
//  It tries to intelligently choose the frame cache size depending on the image and memory situation with the goal to lower CPU usage for smaller ones, lower memory usage for larger ones and always deliver frames for high performant play-back.
//  Note: `posterImage`, `size`, `loopCount`, `delayTimes` and `frameCount` don't change after successful initialization.
//
//@property (nonatomic, strong, readonly) UIImage *posterImage; // Guaranteed to be loaded; usually equivalent to `-imageLazilyCachedAtIndex:0`
//@property (nonatomic, assign, readonly) CGSize size; // The `.posterImage`'s `.size`

//@property(nonatomic,readonly) NSArray *images;
//@property(nonatomic,readonly) NSTimeInterval duration;
@interface FLAnimatedImage : UIImage

@property (nonatomic, assign, readonly) NSUInteger loopCount; // 0 means repeating the animation indefinitely
@property (nonatomic, strong, readonly) NSArray *delayTimes; // Of type `NSTimeInterval` boxed in `NSNumber`s
@property (nonatomic, assign, readonly) NSUInteger frameCount; // Number of valid frames; equal to `[.delayTimes count]`

@property (nonatomic, assign, readonly) NSUInteger frameCacheSizeCurrent; // Current size of intelligently chosen buffer window; can range in the interval [1..frameCount]
@property (nonatomic, assign) NSUInteger frameCacheSizeMax; // Allow to cap the cache size; 0 means no specific limit (default)

@property (nonatomic, strong, readonly) NSData *data; // The data the receiver was initialized with
@property (nonatomic, assign, readonly) FLAnimatedImageInitMode mode; // The mode the receiver was initialized with

#if defined(DEBUG) && DEBUG
// Only intended to report internal state for debugging
@property (nonatomic, weak) id<FLAnimatedImageDebugDelegate> debug_delegate;
@property (nonatomic, strong) NSMutableDictionary *debug_info; // To track arbitrary data (e.g. original URL, loading durations, cache hits, etc.)
#endif

// For initialization use the adopted `UIImage` convenience initializers (e.g. `[FLAnimatedImage imageNamed:@"nyan.gif"]`) or the specific `FLAnimatedImage` initializers (see `FLAnimatedImageInitMode` for details on mode).
+ (instancetype)imageNamed:(NSString *)name;
+ (instancetype)imageNamed:(NSString *)name inBundle:(NSBundle *)bundle compatibleWithTraitCollection:(UITraitCollection *)traitCollection;
+ (instancetype)imageWithContentsOfFile:(NSString *)path;
- (instancetype)initWithImageData:(NSData *)data;
- (instancetype)initWithImageData:(NSData *)data mode:(FLAnimatedImageInitMode)mode;
- (instancetype)initWithImageData:(NSData *)data scale:(CGFloat)scale;
- (instancetype)initWithImageData:(NSData *)data mode:(FLAnimatedImageInitMode)mode scale:(CGFloat)scale;
+ (instancetype)imageWithData:(NSData *)data;
+ (instancetype)imageWithData:(NSData *)data mode:(FLAnimatedImageInitMode)mode;
+ (instancetype)imageWithData:(NSData *)data scale:(CGFloat)scale;
+ (instancetype)imageWithData:(NSData *)data mode:(FLAnimatedImageInitMode)mode scale:(CGFloat)scale;
// Note: The `-[UIImage init*]` and `+[UIImage animated*]` initializers remain unchanged.


// Intended to be called from main thread synchronously; will return immediately.
// If the result isn't cached, will return `nil`; the caller should then pause playback, not increment frame counter and keep polling.
- (UIImage *)imageLazilyCachedAtIndex:(NSUInteger)index;
//#warning Add Comment about how to request frames/performance/cash miss. Note: `-images`

@end


typedef NS_OPTIONS(NSUInteger, FLAnimatedImageOptions) {
    FLAnimatedImageOptionNone           = 0,
    FLAnimatedImageOptionVariableDelays = 1 << 0 // Support variable delays by repeating frames with longer delays.
};


// Create plain animated `UIImage`s (e.g. to set on Apple Watch with `-[WKInterfaceImage setImage:]`).
// Note: Like with the `+[UIImage animated*]` initializers, the returned objects are actually of type `[_UIAnimatedImage]`.
@interface UIImage (FLAnimatedImage)

+ (UIImage *)animatedImageNamed:(NSString *)name; // Defaults to `FLAnimatedImageOptionVariableDelays` for most accurate playback
+ (UIImage *)animatedImageWithData:(NSData *)data options:(FLAnimatedImageOptions)options;

@end


@interface FLWeakProxy : NSProxy

+ (instancetype)weakProxyForObject:(id)targetObject;

@end


#if defined(DEBUG) && DEBUG
@protocol FLAnimatedImageDebugDelegate <NSObject>

@optional

- (void)debug_animatedImage:(FLAnimatedImage *)animatedImage didUpdateCachedFrames:(NSIndexSet *)indexesOfFramesInCache;
- (void)debug_animatedImage:(FLAnimatedImage *)animatedImage didRequestCachedFrame:(NSUInteger)index;
- (CGFloat)debug_animatedImagePredrawingSlowdownFactor:(FLAnimatedImage *)animatedImage;

@end
#endif


// Try to detect and import CocoaLumberjack in all scenarious (library versions, way of including it, CocoaPods versions, etc.).
#if FLLumberjackIntegrationEnabled
    #if defined(__has_include)
        #if __has_include("<CocoaLumberjack/CocoaLumberjack.h>")
            #import <CocoaLumberjack/CocoaLumberjack.h>
        #elif __has_include("CocoaLumberjack.h")
            #import "CocoaLumberjack.h"
        #elif __has_include("<CocoaLumberjack/DDLog.h>")
            #import <CocoaLumberjack/DDLog.h>
        #elif __has_include("DDLog.h")
            #import "DDLog.h"
        #endif
    #elif defined(COCOAPODS_POD_AVAILABLE_CocoaLumberjack) || defined(__POD_CocoaLumberjack)
        #if COCOAPODS_VERSION_MAJOR_CocoaLumberjack == 2
            #import <CocoaLumberjack/CocoaLumberjack.h>
        #else
            #import <CocoaLumberjack/DDLog.h>
        #endif
    #endif

    #if defined(DDLogError) && defined(DDLogWarn) && defined(DDLogInfo) && defined(DDLogDebug) && defined(DDLogVerbose)
        #define FLLumberjackAvailable
    #endif
#endif

#if FLLumberjackIntegrationEnabled && defined(FLLumberjackAvailable)
    // Use a custom, global (not per-file) log level for this library.
    extern int flAnimatedImageLogLevel;
    #if defined(LOG_OBJC_MAYBE) // CocoaLumberjack 1.x
        #define FLLogError(frmt, ...)   LOG_OBJC_MAYBE(LOG_ASYNC_ERROR,   flAnimatedImageLogLevel, LOG_FLAG_ERROR,   0, frmt, ##__VA_ARGS__)
        #define FLLogWarn(frmt, ...)    LOG_OBJC_MAYBE(LOG_ASYNC_WARN,    flAnimatedImageLogLevel, LOG_FLAG_WARN,    0, frmt, ##__VA_ARGS__)
        #define FLLogInfo(frmt, ...)    LOG_OBJC_MAYBE(LOG_ASYNC_INFO,    flAnimatedImageLogLevel, LOG_FLAG_INFO,    0, frmt, ##__VA_ARGS__)
        #define FLLogDebug(frmt, ...)   LOG_OBJC_MAYBE(LOG_ASYNC_DEBUG,   flAnimatedImageLogLevel, LOG_FLAG_DEBUG,   0, frmt, ##__VA_ARGS__)
        #define FLLogVerbose(frmt, ...) LOG_OBJC_MAYBE(LOG_ASYNC_VERBOSE, flAnimatedImageLogLevel, LOG_FLAG_VERBOSE, 0, frmt, ##__VA_ARGS__)
    #else // CocoaLumberjack 2.x
        #define FLLogError(frmt, ...)   LOG_MAYBE(NO,                flAnimatedImageLogLevel, DDLogFlagError,   0, nil, __PRETTY_FUNCTION__, frmt, ##__VA_ARGS__)
        #define FLLogWarn(frmt, ...)    LOG_MAYBE(LOG_ASYNC_ENABLED, flAnimatedImageLogLevel, DDLogFlagWarning, 0, nil, __PRETTY_FUNCTION__, frmt, ##__VA_ARGS__)
        #define FLLogInfo(frmt, ...)    LOG_MAYBE(LOG_ASYNC_ENABLED, flAnimatedImageLogLevel, DDLogFlagInfo,    0, nil, __PRETTY_FUNCTION__, frmt, ##__VA_ARGS__)
        #define FLLogDebug(frmt, ...)   LOG_MAYBE(LOG_ASYNC_ENABLED, flAnimatedImageLogLevel, DDLogFlagDebug,   0, nil, __PRETTY_FUNCTION__, frmt, ##__VA_ARGS__)
        #define FLLogVerbose(frmt, ...) LOG_MAYBE(LOG_ASYNC_ENABLED, flAnimatedImageLogLevel, DDLogFlagVerbose, 0, nil, __PRETTY_FUNCTION__, frmt, ##__VA_ARGS__)
    #endif
#else
    #if FLDebugLoggingEnabled && DEBUG
        // CocoaLumberjack is disabled or not available, but we want to fallback to regular logging (debug builds only).
        #define FLLog(...) NSLog(__VA_ARGS__)
    #else
        // No logging at all.
        #define FLLog(...) ((void)0)
    #endif
    #define FLLogError(...)   FLLog(__VA_ARGS__)
    #define FLLogWarn(...)    FLLog(__VA_ARGS__)
    #define FLLogInfo(...)    FLLog(__VA_ARGS__)
    #define FLLogDebug(...)   FLLog(__VA_ARGS__)
    #define FLLogVerbose(...) FLLog(__VA_ARGS__)
#endif
