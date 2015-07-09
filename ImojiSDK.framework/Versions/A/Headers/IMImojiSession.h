//
// Created by Nima on 4/6/15.
// Copyright (c) 2015 Builds, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <CoreGraphics/CoreGraphics.h>
#import "IMImojiObjectRenderingOptions.h"

@class IMImojiObject, IMImojiSessionStoragePolicy;
@protocol IMImojiSessionDelegate;

/**
* @abstract The error domain used within NSError objects generated by IMImojiSession
*/
extern NSString *IMImojiSessionErrorDomain;

/**
* @abstract Error codes used by errors returned within IMImojiSession
*/
typedef NS_ENUM(NSUInteger, IMImojiSessionState) {
    /**
    * @abstract The session is not currently connected with the imoji server
    */
    IMImojiSessionStateNotConnected,

    /**
    * @abstract The session is connected with the server but not synchronized with a user account
    */
    IMImojiSessionStateConnected,

    /**
    * @abstract The session is connected with the server and synchronized with a user account
    */
    IMImojiSessionStateConnectedSynchronized
};

/**
* @abstract Error codes used by errors returned within IMImojiSession
*/
typedef NS_ENUM(NSUInteger, IMImojiSessionErrorCode) {
    /**
    * @abstract Used when the supplied client id and api token are not recognized by the server
    */
    IMImojiSessionErrorCodeInvalidCredentials,
    /**
    * @abstract Used as a fallback when the server returns an error and no other specific error code exists
    */
    IMImojiSessionErrorCodeServerError,
    /**
    * @abstract Used when the consumer supplies a corrupt IMImojiObject to IMImojiSession for rendering
    */
    IMImojiSessionErrorCodeImojiDoesNotExist,
    /**
    * @abstract Used whenever the consumer sends a bad argument to any method in IMImojiSession
    */
    IMImojiSessionErrorCodeInvalidArgument,
    /**
    * @abstract Used when IMImojiSession is unable to render the IMImojiObject to a UIImage
    */
    IMImojiSessionErrorCodeInvalidImage,
    /**
    * @abstract Used when IMImojiSession is unable to fetch a users set of imojis because the session is not synchronized
    */
    IMImojiSessionErrorCodeSessionNotSynchronized,
    /**
    * @abstract Used when IMImojiSession is unable to authenticate a session with a user
    */
    IMImojiSessionErrorCodeUserAuthenticationFailed,
    /**
    * @abstract Used when IMImojiSession is unable to carry a request because the Imoji application is not installed
    */
    IMImojiSessionErrorCodeImojiApplicationNotInstalled,
    /**
    * @abstract Used when IMImojiSession is unable to render the IMImojiObject
    */
    IMImojiSessionErrorCodeImojiRenderingUnavailable
};

/**
* @abstract Defines a high level grouping of category types
*/
typedef NS_ENUM(NSUInteger, IMImojiSessionCategoryClassification) {
    /**
    * @abstract Allows for the caller to obtain all trending and time sensitive categories
    * (ex: sporting events, holidays, etc).
    */
    IMImojiSessionCategoryClassificationTrending,

    /**
    * @abstract Allows for the caller to obtain categories of imojis that are not time sensitive
    * (ex: emotions, locations, people, etc).
    */
    IMImojiSessionCategoryClassificationGeneric
};

/**
* @abstract Callback used for triggering when the server has loaded a result set
* @param resultCount Number of results returned by the server. This can never be nil.
* @param error An error with code equal to an IMImojiSessionErrorCode value or nil if the request succeeded
*/
typedef void (^IMImojiSessionResultSetResponseCallback)(NSNumber *resultCount, NSError *error);

/**
* @abstract Callback used for generic asynchronous requests
* @param imoji The fetched IMImojiObject
* @param index Position in the results set the imoji belongs to
* @param error An error with code equal to an IMImojiSessionErrorCode value or nil if the request succeeded
*/
typedef void (^IMImojiSessionImojiFetchedResponseCallback)(IMImojiObject *imoji, NSUInteger index, NSError *error);

/**
* @abstract Callback used for generic asynchronous requests
* @param imojiCategories An array of IMImojiCategoryObject's
* @param error An error with code equal to an IMImojiSessionErrorCode value or nil if the request succeeded
*/
typedef void (^IMImojiSessionImojiCategoriesResponseCallback)(NSArray *imojiCategories, NSError *error);

/**
* @abstract Callback triggered when an imoji has been rendered as an UIImage
* @param image UIImage representation of the IMImojiObject
* @param error An error with code equal to an IMImojiSessionErrorCode value or nil if the request succeeded
*/
typedef void (^IMImojiSessionImojiRenderResponseCallback)(UIImage *image, NSError *error);

/**
* @abstract Callback used for generic asynchronous requests
* @param successful Whether or not the operation succeed
* @param error An error with code equal to an IMImojiSessionErrorCode value or nil if the request succeeded
*/
typedef void (^IMImojiSessionAsyncResponseCallback)(BOOL successful, NSError *error);

@interface IMImojiSession : NSObject

/**
* @abstract Creates a imoji session object.
* @param storagePolicy The storage policy to use for persisting imojis.
*/
- (instancetype)initWithStoragePolicy:(IMImojiSessionStoragePolicy *)storagePolicy;

/**
* @abstract Creates a imoji session object with a default temporary file system storage policy.
*/
+ (instancetype)imojiSession;

/**
* @abstract Creates a imoji session object.
* @param storagePolicy The storage policy to use for persisting imojis.
*/
+ (instancetype)imojiSessionWithStoragePolicy:(IMImojiSessionStoragePolicy *)storagePolicy;

/**
* @abstract The current state of the session
*/
@property(readonly) IMImojiSessionState sessionState;

/**
* @abstract An optional session delegate to receive notifications when session information changes
*/
@property(nonatomic, strong) id <IMImojiSessionDelegate> delegate;

/**
* @abstract An optional cache instance to be used for optimizing rendering calls
*/
@property(nonatomic, strong) NSCache *contentCache;

@end

/**
* @abstract Main session object for making requests that require server access with the Imoji server.
*/
@interface IMImojiSession (ImojiFetching)

/**
* @abstract Fetches top level imoji categories.
* DEPRECATED: Use getImojiCategoriesWithClassification:callback instead
* @param callback Block callback to call when categories have been downloaded.
* @return An operation reference that can be used to cancel the request.
*/
- (NSOperation *)getImojiCategories:(IMImojiSessionImojiCategoriesResponseCallback)callback __deprecated;

/**
* @abstract Fetches top level imoji categories given a classification type.
* @param classification Type of category classification to retrieve
* @param callback Block callback to call when categories have been downloaded.
* @return An operation reference that can be used to cancel the request.
*/
- (NSOperation *)getImojiCategoriesWithClassification:(IMImojiSessionCategoryClassification)classification
                                             callback:(IMImojiSessionImojiCategoriesResponseCallback)callback;

/**
* @abstract Searches the imojis database with a given search term. The resultSetResponseCallback block is called once the results are available.
* Imoji contents are downloaded individually and imojiResponseCallback is called once the thumbnail of that imoji has been downloaded.
* @param searchTerm Search term to find imojis with. If nil or empty, the server will typically returned the featured set of imojis (this is subject to change).
* @param offset The result offset from a previous search. This may be nil.
* @param numberOfResults Number of results to fetch. This can be nil.
* @param resultSetResponseCallback Callback triggered when the search results are available or if an error occurred.
* @param imojiResponseCallback Callback triggered when an imoji is available to render.
* @return An operation reference that can be used to cancel the request.
*/
- (NSOperation *)searchImojisWithTerm:(NSString *)searchTerm
                               offset:(NSNumber *)offset
                      numberOfResults:(NSNumber *)numberOfResults
            resultSetResponseCallback:(IMImojiSessionResultSetResponseCallback)resultSetResponseCallback
                imojiResponseCallback:(IMImojiSessionImojiFetchedResponseCallback)imojiResponseCallback;

/**
* @abstract Gets a random set of featured imojis. The resultSetResponseCallback block is called once the results are available.
* Imoji contents are downloaded individually and imojiResponseCallback is called once the thumbnail of that imoji has been downloaded.
* @param numberOfResults Number of results to fetch. This can be nil.
* @param resultSetResponseCallback Callback triggered when the featured results are available or if an error occurred.
* @param imojiResponseCallback Callback triggered when an imoji is available to render.
* @return An operation reference that can be used to cancel the request.
*/
- (NSOperation *)getFeaturedImojisWithNumberOfResults:(NSNumber *)numberOfResults
                            resultSetResponseCallback:(IMImojiSessionResultSetResponseCallback)resultSetResponseCallback
                                imojiResponseCallback:(IMImojiSessionImojiFetchedResponseCallback)imojiResponseCallback;

/**
* @abstract Gets corresponding IMImojiObject's for one or more imoji identifiers as NSString's
* Imoji contents are downloaded individually and fetchedResponseCallback is called once the thumbnail of that imoji has been downloaded.
* @param imojiObjectIdentifiers An array of NSString's representing the identifiers of the imojis to fetch
* @param fetchedResponseCallback Callback triggered when an imoji is available to render
* @return An operation reference that can be used to cancel the request.
*/
- (NSOperation *)fetchImojisByIdentifiers:(NSArray *)imojiObjectIdentifiers
                  fetchedResponseCallback:(IMImojiSessionImojiFetchedResponseCallback)fetchedResponseCallback;

@end


@interface IMImojiSession (ImojiDisplaying)

/**
* @abstract Renders an imoji object into a image with a specified border and shadow.
* The imoji image is scaled to fit the specified target size. This may make a server call depending on the availability
* of the imoji with the session storage policy.
* @param imoji The imoji to render.
* @param options Set of options to render the imoji with
* @param callback Called once the imoji UIImage has been rendered
* @return An operation reference that can be used to cancel the request.
*/
- (NSOperation *)renderImoji:(IMImojiObject *)imoji
                     options:(IMImojiObjectRenderingOptions *)options
                    callback:(IMImojiSessionImojiRenderResponseCallback)callback;

@end

@interface IMImojiSession (UserSynchronization)

/**
* @abstract Attempts to synchronize a session with the iOS Imoji application. This method will open the Imoji application
* and either ask the user to register/login if there is no active session, or grant the logged in user access to the
* allow ImojiSDK to access it's information. If the application is not installed or does not support authorization,
* an error is returned in the callback.
* @param error If an error occurred while trying to load the Imoji iOS application
*/
- (void)requestUserSynchronizationWithError:(NSError **)error;

/**
* @abstract Used to determine if the launched URL from application:openURL:sourceApplication:annotation in
* UIApplicationDelegate is originated by the ImojiSDK
* @param url URL from application:openURL:sourceApplication:annotation
* @param sourceApplication If an error occurred while trying to load the Imoji iOS application
* @see [UIApplicationDelegate application:openURL:sourceApplication:annotation:](https://developer.apple.com/library/ios/documentation/UIKit/Reference/UIApplicationDelegate_Protocol/index.html#//apple_ref/occ/intfm/UIApplicationDelegate/application:openURL:sourceApplication:annotation:)
*/
- (BOOL)isImojiAppRequest:(NSURL *)url sourceApplication:(NSString *)sourceApplication;

/**
* @abstract Handles the URL passed to application:openURL:sourceApplication:annotation for authentication
* @param url URL from UIApplicationDelegate application:openURL:sourceApplication:annotation
* @param sourceApplication sourceApplication from UIApplicationDelegate application:openURL:sourceApplication:annotation
* @see [UIApplicationDelegate application:openURL:sourceApplication:annotation:](https://developer.apple.com/library/ios/documentation/UIKit/Reference/UIApplicationDelegate_Protocol/index.html#//apple_ref/occ/intfm/UIApplicationDelegate/application:openURL:sourceApplication:annotation:)
*/
- (BOOL)handleImojiAppRequest:(NSURL *)url sourceApplication:(NSString *)sourceApplication;

/**
* @abstract Removes the synchronization state from the session. Upon calling this method
* @param callback Status callback triggered when the routine is finished. This can be nil.
*/
- (void)clearUserSynchronizationStatus:(IMImojiSessionAsyncResponseCallback)callback;

@end

@interface IMImojiSession (SynchronizedUserActions)

/**
* @abstract Gets imojis associated to the synchronized user account. The sessionState must be IMImojiSessionStateConnectedSynchronized
* in order to receive user imojis.
* @param resultSetResponseCallback Callback triggered when the results are available or if an error occurred.
* @param imojiResponseCallback Callback triggered when an imoji is available to render.
* @return An operation reference that can be used to cancel the request.
*/
- (NSOperation *)getImojisForAuthenticatedUserWithResultSetResponseCallback:(IMImojiSessionResultSetResponseCallback)resultSetResponseCallback
                                                      imojiResponseCallback:(IMImojiSessionImojiFetchedResponseCallback)imojiResponseCallback;

/**
* @abstract Adds a given IMImojiObject to a users collection which is also synchronized with their account.
* The sessionState must be IMImojiSessionStateConnectedSynchronized in order to receive user imojis.
* @param imojiObject The Imoji object to save to the users collection
* @param callback Called once the save operation is complete
* @return An operation reference that can be used to cancel the request.
*/
- (NSOperation *)addImojiToUserCollection:(IMImojiObject *)imojiObject
                                 callback:(IMImojiSessionAsyncResponseCallback)callback;

@end


/**
* @abstract Delegate protocol for IMImojiSession
*/
@protocol IMImojiSessionDelegate <NSObject>

@optional

/**
* @abstract Triggered when the session state changes
* @param session The session in use
* @param newState The current state
* @param oldState The previous state
*/
- (void)imojiSession:(IMImojiSession *)session stateChanged:(IMImojiSessionState)newState fromState:(IMImojiSessionState)oldState;

@end
