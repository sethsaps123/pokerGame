//
//  Networking.swift
//  yakChat
//
//  Created by Seth Saperstein on 2/28/18.
//  Copyright © 2018 Seth Saperstein. All rights reserved.
//

import Foundation

struct Login: Codable {
    let user: String
}

struct Likes: Codable {
    var logname_likes_this: Bool
    var logname_dislikes_this: Bool
    var net_likes: Int
    let postid: Int
}

struct Post: Codable {
    let age: String
    let url: String
    let post_show_url: String
    let text: String
    var likes: Int
}

struct Results: Codable {
    let postid: Int
    let url: String
}

struct RecentPosts: Codable {
    let next: String
    let results: [Results]
}

enum Result<Value> {
    case success(Value)
    case failure(Error)
}

struct NewPost: Codable {
    let text: String
}

struct SubmitLikeDislike: Codable {
    let postid: Int
}


func submitLogin(login: Login, completion:((Error?) -> Void)?) {

    // Specify this request as being a POST method
    var request = URLRequest(url: URL(string: "http://0.0.0.0:8000/api/v1/accounts/login/")!)
    request.httpMethod = "POST"
    // Make sure that we include headers specifying that our request's HTTP body
    // will be JSON encoded
    var headers = request.allHTTPHeaderFields ?? [:]
    headers["Content-Type"] = "application/json"
    request.allHTTPHeaderFields = headers
    
    // Now let's encode out Post struct into JSON data...
    let encoder = JSONEncoder()
    do {
        let jsonData = try encoder.encode(login)
        // ... and set our request's HTTP body
        request.httpBody = jsonData
        print("jsonData: ", String(data: request.httpBody!, encoding: .utf8) ?? "no body data")
    } catch {
        completion?(error)
    }
    
    // Create and run a URLSession data task with our JSON encoded POST request
    let config = URLSessionConfiguration.default
    let session = URLSession(configuration: config)
    let task = session.dataTask(with: request) { (responseData, response, responseError) in
        guard responseError == nil else {
            completion?(responseError!)
            return
        }
        
        // APIs usually respond with the data you just sent in your POST request
        if let data = responseData, let utf8Representation = String(data: data, encoding: .utf8) {
            print("response: ", utf8Representation)
        } else {
            print("no readable data received in response")
        }
    }
    task.resume()
}

func getRecentPosts(type: String, completion: ((Result<RecentPosts>) -> Void)?) {
    
    var request = URLRequest(url: URL(string: "http://0.0.0.0:8000/api/v1/p/?type=new")!)
    if type == "hot" {
        request = URLRequest(url: URL(string: "http://0.0.0.0:8000/api/v1/p/?type=hot")!)
    }
    request.httpMethod = "GET"
    
    let config = URLSessionConfiguration.default
    let session = URLSession(configuration: config)
    let task = session.dataTask(with: request) { (responseData, response, responseError) in
        DispatchQueue.main.async {
            if let error = responseError {
                completion?(.failure(error))
            } else if let jsonData = responseData {
                // Now we have jsonData, Data representation of the JSON returned to us
                // from our URLRequest...
                
                // Create an instance of JSONDecoder to decode the JSON data to our
                // Codable struct
                let decoder = JSONDecoder()
                
                do {
                    // We would use Post.self for JSON representing a single Post
                    // object, and [Post].self for JSON representing an array of
                    // Post objects
                    let posts = try decoder.decode(RecentPosts.self, from: jsonData)
                    completion?(.success(posts))
                } catch {
                    completion?(.failure(error))
                }
            } else {
                let error = NSError(domain: "", code: 0, userInfo: [NSLocalizedDescriptionKey : "Data was not retrieved from request"]) as Error
                completion?(.failure(error))
            }
        }
    }
    
    task.resume()
}

func getPosts(for postid: String, completion: ((Result<Post>) -> Void)?) {

    var request = URLRequest(url: URL(string: "http://0.0.0.0:8000/api/v1/p/\(postid)/")!)
    request.httpMethod = "GET"
    print(request.url!)
    let config = URLSessionConfiguration.default
    let session = URLSession(configuration: config)
    let task = session.dataTask(with: request) { (responseData, response, responseError) in
        DispatchQueue.main.async {
            if let error = responseError {
                completion?(.failure(error))
            } else if let jsonData = responseData {
                if let data = responseData, let utf8Representation = String(data: data, encoding: .utf8) {
                    print("response: ", utf8Representation)
                }
                // Now we have jsonData, Data representation of the JSON returned to us
                // from our URLRequest...
                
                // Create an instance of JSONDecoder to decode the JSON data to our
                // Codable struct
                let decoder = JSONDecoder()
                
                do {
                    // We would use Post.self for JSON representing a single Post
                    // object, and [Post].self for JSON representing an array of
                    // Post objects
                    let posts = try decoder.decode(Post.self, from: jsonData)
                    completion?(.success(posts))
                } catch {
                    completion?(.failure(error))
                }
            } else {
                let error = NSError(domain: "", code: 0, userInfo: [NSLocalizedDescriptionKey : "Data was not retrieved from request"]) as Error
                completion?(.failure(error))
            }
        }
    }
    
    task.resume()
}

func submitPost(post: NewPost, completion:((Result<Post>?) -> Void)?) {
    
    // Specify this request as being a POST method
    var request = URLRequest(url: URL(string: "http://0.0.0.0:8000/api/v1/p/create/")!)
    request.httpMethod = "POST"
    // Make sure that we include headers specifying that our request's HTTP body
    // will be JSON encoded
    var headers = request.allHTTPHeaderFields ?? [:]
    headers["Content-Type"] = "application/json"
    request.allHTTPHeaderFields = headers
    
    // Now let's encode out Post struct into JSON data...
    let encoder = JSONEncoder()
    do {
        let jsonData = try encoder.encode(post)
        // ... and set our request's HTTP body
        request.httpBody = jsonData
        print("jsonData: ", String(data: request.httpBody!, encoding: .utf8) ?? "no body data")
    } catch {
        completion?(.failure(error))
    }
    
    // Create and run a URLSession data task with our JSON encoded POST request
    let config = URLSessionConfiguration.default
    let session = URLSession(configuration: config)
    let task = session.dataTask(with: request) { (responseData, response, responseError) in
        guard responseError == nil else {
            if let error = responseError {
                completion?(.failure(error))
            }
            return
        }
        
        // APIs usually respond with the data you just sent in your POST request
        if let data = responseData, let utf8Representation = String(data: data, encoding: .utf8) {
            print("response: ", utf8Representation)
            if let jsonData = responseData {
                // Now we have jsonData, Data representation of the JSON returned to us
                // from our URLRequest...
                
                // Create an instance of JSONDecoder to decode the JSON data to our
                // Codable struct
                let decoder = JSONDecoder()
                
                do {
                    // We would use Post.self for JSON representing a single Post
                    // object, and [Post].self for JSON representing an array of
                    // Post objects
                    let post = try decoder.decode(Post.self, from: jsonData)
                    completion?(.success(post))
                } catch {
                    completion?(.failure(error))
                }
            }
        } else {
            print("no readable data received in response")
        }
    }
    task.resume()
}

func getLikeInfo(postid: String, completion: ((Result<Likes>) -> Void)?) {
    
    var request = URLRequest(url: URL(string: "http://0.0.0.0:8000/api/v1/p/\(postid)/likes/")!)

    request.httpMethod = "GET"
    
    let config = URLSessionConfiguration.default
    let session = URLSession(configuration: config)
    let task = session.dataTask(with: request) { (responseData, response, responseError) in
        DispatchQueue.main.async {
            if let error = responseError {
                completion?(.failure(error))
            } else if let jsonData = responseData {
                if let data = responseData, let utf8Representation = String(data: data, encoding: .utf8) {
                    print("response: ", utf8Representation)
                }
                
                // Now we have jsonData, Data representation of the JSON returned to us
                // from our URLRequest...
                
                // Create an instance of JSONDecoder to decode the JSON data to our
                // Codable struct
                let decoder = JSONDecoder()
                
                do {
                    // We would use Post.self for JSON representing a single Post
                    // object, and [Post].self for JSON representing an array of
                    // Post objects
                    let likeInfo = try decoder.decode(Likes.self, from: jsonData)
                    completion?(.success(likeInfo))
                } catch {
                    completion?(.failure(error))
                }
            } else {
                let error = NSError(domain: "", code: 0, userInfo: [NSLocalizedDescriptionKey : "Data was not retrieved from request"]) as Error
                completion?(.failure(error))
            }
        }
    }
    
    task.resume()
}

func submitLikeDislike(postid: String, like: Bool, dislike: Bool, completion:((Result<SubmitLikeDislike>) -> Void)?) {
    // can only do one at a time so check if sent both
    if like && dislike {
        fatalError("Cannot like and dislike at same time")
    }

    var request = URLRequest(url: URL(string: "http://0.0.0.0:8000/api/v1/p/\(postid)/likes/")!)
    if dislike {
        request = URLRequest(url: URL(string: "http://0.0.0.0:8000/api/v1/p/\(postid)/dislikes/")!)
    }
    
    let newLikeDislike = SubmitLikeDislike(postid: Int(postid)!)
    
    request.httpMethod = "POST"
    // Make sure that we include headers specifying that our request's HTTP body
    // will be JSON encoded
    var headers = request.allHTTPHeaderFields ?? [:]
    headers["Content-Type"] = "application/json"
    request.allHTTPHeaderFields = headers
    
    // Now let's encode out Post struct into JSON data...
    let encoder = JSONEncoder()
    do {
        let jsonData = try encoder.encode(newLikeDislike)
        // ... and set our request's HTTP body
        request.httpBody = jsonData
        print("jsonData: ", String(data: request.httpBody!, encoding: .utf8) ?? "no body data")
    } catch {
        completion?(.failure(error))
    }
    
    // Create and run a URLSession data task with our JSON encoded POST request
    let config = URLSessionConfiguration.default
    let session = URLSession(configuration: config)
    let task = session.dataTask(with: request) { (responseData, response, responseError) in
        guard responseError == nil else {
            if let error = responseError {
                completion?(.failure(error))
            }
            return
        }
        
        // APIs usually respond with the data you just sent in your POST request
        if let data = responseData, let utf8Representation = String(data: data, encoding: .utf8) {
            print("response: ", utf8Representation)
            if let jsonData = responseData {
                // Now we have jsonData, Data representation of the JSON returned to us
                // from our URLRequest...
                
                // Create an instance of JSONDecoder to decode the JSON data to our
                // Codable struct
                let decoder = JSONDecoder()
                
                do {
                    // We would use Post.self for JSON representing a single Post
                    // object, and [Post].self for JSON representing an array of
                    // Post objects
                    let like = try decoder.decode(SubmitLikeDislike.self, from: jsonData)
                    completion?(.success(like))
                } catch {
                    completion?(.failure(error))
                }
            }
        } else {
            print("no readable data received in response")
        }
    }
    task.resume()
}

func deleteLikeDislike(postid: String, like: Bool, dislike: Bool, completion:((Result<SubmitLikeDislike>) -> Void)?) {
    // can only do one at a time so check if sent both
    if like && dislike {
        fatalError("Cannot like and dislike at same time")
    }
    
    var request = URLRequest(url: URL(string: "http://0.0.0.0:8000/api/v1/p/\(postid)/likes/")!)
    if dislike {
        request = URLRequest(url: URL(string: "http://0.0.0.0:8000/api/v1/p/\(postid)/dislikes/")!)
    }
    
    let newLikeDislike = SubmitLikeDislike(postid: Int(postid)!)
    
    request.httpMethod = "DELETE"
    // Make sure that we include headers specifying that our request's HTTP body
    // will be JSON encoded
    var headers = request.allHTTPHeaderFields ?? [:]
    headers["Content-Type"] = "application/json"
    request.allHTTPHeaderFields = headers
    
    // Now let's encode out Post struct into JSON data...
    let encoder = JSONEncoder()
    do {
        let jsonData = try encoder.encode(newLikeDislike)
        // ... and set our request's HTTP body
        request.httpBody = jsonData
        print("jsonData: ", String(data: request.httpBody!, encoding: .utf8) ?? "no body data")
    } catch {
        completion?(.failure(error))
    }
    
    // Create and run a URLSession data task with our JSON encoded POST request
    let config = URLSessionConfiguration.default
    let session = URLSession(configuration: config)
    let task = session.dataTask(with: request) { (responseData, response, responseError) in
        guard responseError == nil else {
            if let error = responseError {
                completion?(.failure(error))
            }
            return
        }
        
        // APIs usually respond with the data you just sent in your POST request
        if let data = responseData, let utf8Representation = String(data: data, encoding: .utf8) {
            print("response: ", utf8Representation)
            if let jsonData = responseData {
                // Now we have jsonData, Data representation of the JSON returned to us
                // from our URLRequest...
                
                // Create an instance of JSONDecoder to decode the JSON data to our
                // Codable struct
                let decoder = JSONDecoder()
                
                do {
                    // We would use Post.self for JSON representing a single Post
                    // object, and [Post].self for JSON representing an array of
                    // Post objects
                    let like = try decoder.decode(SubmitLikeDislike.self, from: jsonData)
                    completion?(.success(like))
                } catch {
                    completion?(.failure(error))
                }
            }
        } else {
            print("no readable data received in response")
        }
    }
    task.resume()
}

func getAllComments(posturl: String, completion: ((Result<RecentPosts>) -> Void)?) {
    
    var request = URLRequest(url: URL(string: "http://0.0.0.0:8000" + posturl + "comments/")!)
    request.httpMethod = "GET"
    
    let config = URLSessionConfiguration.default
    let session = URLSession(configuration: config)
    let task = session.dataTask(with: request) { (responseData, response, responseError) in
        DispatchQueue.main.async {
            if let error = responseError {
                completion?(.failure(error))
            } else if let jsonData = responseData {
                // Now we have jsonData, Data representation of the JSON returned to us
                // from our URLRequest...
                
                // Create an instance of JSONDecoder to decode the JSON data to our
                // Codable struct
                let decoder = JSONDecoder()
                do {
                    // We would use Post.self for JSON representing a single Post
                    // object, and [Post].self for JSON representing an array of
                    // Post objects
                    let posts = try decoder.decode(RecentPosts.self, from: jsonData)
                    completion?(.success(posts))
                } catch {
                    completion?(.failure(error))
                }
            } else {
                let error = NSError(domain: "", code: 0, userInfo: [NSLocalizedDescriptionKey : "Data was not retrieved from request"]) as Error
                completion?(.failure(error))
            }
        }
    }
    
    task.resume()
}

func getComment(for posturl: String, for commentid: String, completion: ((Result<Post>) -> Void)?) {
    
    var request = URLRequest(url: URL(string: "http://0.0.0.0:8000" + posturl + "comments/\(commentid)/")!)
    request.httpMethod = "GET"
    
    let config = URLSessionConfiguration.default
    let session = URLSession(configuration: config)
    let task = session.dataTask(with: request) { (responseData, response, responseError) in
        DispatchQueue.main.async {
            if let error = responseError {
                completion?(.failure(error))
            } else if let jsonData = responseData {
                if let data = responseData, let utf8Representation = String(data: data, encoding: .utf8) {
                    print("response: ", utf8Representation)
                }
                // Now we have jsonData, Data representation of the JSON returned to us
                // from our URLRequest...
                
                // Create an instance of JSONDecoder to decode the JSON data to our
                // Codable struct
                let decoder = JSONDecoder()
                
                do {
                    // We would use Post.self for JSON representing a single Post
                    // object, and [Post].self for JSON representing an array of
                    // Post objects
                    let posts = try decoder.decode(Post.self, from: jsonData)
                    completion?(.success(posts))
                } catch {
                    completion?(.failure(error))
                }
            } else {
                let error = NSError(domain: "", code: 0, userInfo: [NSLocalizedDescriptionKey : "Data was not retrieved from request"]) as Error
                completion?(.failure(error))
            }
        }
    }
    
    task.resume()
}

func getCommentLikeInfo(posturl: String, commentid: String, completion: ((Result<Likes>) -> Void)?) {
    
    var request = URLRequest(url: URL(string: "http://0.0.0.0:8000" + posturl + "comments/\(commentid)/likes/")!)
    
    request.httpMethod = "GET"
    print(request.url!)
    let config = URLSessionConfiguration.default
    let session = URLSession(configuration: config)
    let task = session.dataTask(with: request) { (responseData, response, responseError) in
        DispatchQueue.main.async {
            if let error = responseError {
                completion?(.failure(error))
            } else if let jsonData = responseData {
                if let data = responseData, let utf8Representation = String(data: data, encoding: .utf8) {
                    print("response: ", utf8Representation)
                }
                
                // Now we have jsonData, Data representation of the JSON returned to us
                // from our URLRequest...
                
                // Create an instance of JSONDecoder to decode the JSON data to our
                // Codable struct
                let decoder = JSONDecoder()
                
                do {
                    // We would use Post.self for JSON representing a single Post
                    // object, and [Post].self for JSON representing an array of
                    // Post objects
                    let likeInfo = try decoder.decode(Likes.self, from: jsonData)
                    completion?(.success(likeInfo))
                } catch {
                    completion?(.failure(error))
                }
            } else {
                let error = NSError(domain: "", code: 0, userInfo: [NSLocalizedDescriptionKey : "Data was not retrieved from request"]) as Error
                completion?(.failure(error))
            }
        }
    }
    
    task.resume()
}

func submitComment(comment: NewPost, posturl: String, completion:((Result<Post>?) -> Void)?) {
    
    // Specify this request as being a POST method
    var request = URLRequest(url: URL(string: "http://0.0.0.0:8000" + posturl + "comments/create/")!)
    print(request.url!)
    request.httpMethod = "POST"
    // Make sure that we include headers specifying that our request's HTTP body
    // will be JSON encoded
    var headers = request.allHTTPHeaderFields ?? [:]
    headers["Content-Type"] = "application/json"
    request.allHTTPHeaderFields = headers
    
    // Now let's encode out Post struct into JSON data...
    let encoder = JSONEncoder()
    do {
        let jsonData = try encoder.encode(comment)
        // ... and set our request's HTTP body
        request.httpBody = jsonData
        print("jsonData: ", String(data: request.httpBody!, encoding: .utf8) ?? "no body data")
    } catch {
        completion?(.failure(error))
    }
    
    // Create and run a URLSession data task with our JSON encoded POST request
    let config = URLSessionConfiguration.default
    let session = URLSession(configuration: config)
    let task = session.dataTask(with: request) { (responseData, response, responseError) in
        guard responseError == nil else {
            if let error = responseError {
                completion?(.failure(error))
            }
            return
        }
        
        // APIs usually respond with the data you just sent in your POST request
        if let data = responseData, let utf8Representation = String(data: data, encoding: .utf8) {
            print("response: ", utf8Representation)
            if let jsonData = responseData {
                // Now we have jsonData, Data representation of the JSON returned to us
                // from our URLRequest...
                
                // Create an instance of JSONDecoder to decode the JSON data to our
                // Codable struct
                let decoder = JSONDecoder()
                
                do {
                    // We would use Post.self for JSON representing a single Post
                    // object, and [Post].self for JSON representing an array of
                    // Post objects
                    let post = try decoder.decode(Post.self, from: jsonData)
                    completion?(.success(post))
                } catch {
                    completion?(.failure(error))
                }
            }
        } else {
            print("no readable data received in response")
        }
    }
    task.resume()
}
func submitCommentLikeDislike(posturl: String, commentid: String, like: Bool, dislike: Bool, completion:((Result<SubmitLikeDislike>) -> Void)?) {
    // can only do one at a time so check if sent both
    if like && dislike {
        fatalError("Cannot like and dislike at same time")
    }
    
    var request = URLRequest(url: URL(string: "http://0.0.0.0:8000" + posturl + "comments/\(commentid)/likes/")!)
    if dislike {
        request = URLRequest(url: URL(string: "http://0.0.0.0:8000" + posturl + "comments/\(commentid)/dislikes/")!)
    }
    print(request.url!)
    
    let newLikeDislike = SubmitLikeDislike(postid: Int(commentid)!)
    
    request.httpMethod = "POST"
    // Make sure that we include headers specifying that our request's HTTP body
    // will be JSON encoded
    var headers = request.allHTTPHeaderFields ?? [:]
    headers["Content-Type"] = "application/json"
    request.allHTTPHeaderFields = headers
    
    // Now let's encode out Post struct into JSON data...
    let encoder = JSONEncoder()
    do {
        let jsonData = try encoder.encode(newLikeDislike)
        // ... and set our request's HTTP body
        request.httpBody = jsonData
        print("jsonData: ", String(data: request.httpBody!, encoding: .utf8) ?? "no body data")
    } catch {
        completion?(.failure(error))
    }
    
    // Create and run a URLSession data task with our JSON encoded POST request
    let config = URLSessionConfiguration.default
    let session = URLSession(configuration: config)
    let task = session.dataTask(with: request) { (responseData, response, responseError) in
        guard responseError == nil else {
            if let error = responseError {
                completion?(.failure(error))
            }
            return
        }
        
        // APIs usually respond with the data you just sent in your POST request
        if let data = responseData, let utf8Representation = String(data: data, encoding: .utf8) {
            print("response: ", utf8Representation)
            if let jsonData = responseData {
                // Now we have jsonData, Data representation of the JSON returned to us
                // from our URLRequest...
                
                // Create an instance of JSONDecoder to decode the JSON data to our
                // Codable struct
                let decoder = JSONDecoder()
                
                do {
                    // We would use Post.self for JSON representing a single Post
                    // object, and [Post].self for JSON representing an array of
                    // Post objects
                    let like = try decoder.decode(SubmitLikeDislike.self, from: jsonData)
                    completion?(.success(like))
                } catch {
                    completion?(.failure(error))
                }
            }
        } else {
            print("no readable data received in response")
        }
    }
    task.resume()
}

func deleteCommentLikeDislike(posturl: String, commentid: String, like: Bool, dislike: Bool, completion:((Result<SubmitLikeDislike>) -> Void)?) {
    // can only do one at a time so check if sent both
    if like && dislike {
        fatalError("Cannot like and dislike at same time")
    }
    
    var request = URLRequest(url: URL(string: "http://0.0.0.0:8000" + posturl + "comments/\(commentid)/likes/")!)
    if dislike {
        request = URLRequest(url: URL(string: "http://0.0.0.0:8000" + posturl + "comments/\(commentid)/dislikes/")!)
    }
    
    print(request.url!)
    
    let newLikeDislike = SubmitLikeDislike(postid: Int(commentid)!)
    
    request.httpMethod = "DELETE"
    // Make sure that we include headers specifying that our request's HTTP body
    // will be JSON encoded
    var headers = request.allHTTPHeaderFields ?? [:]
    headers["Content-Type"] = "application/json"
    request.allHTTPHeaderFields = headers
    
    // Now let's encode out Post struct into JSON data...
    let encoder = JSONEncoder()
    do {
        let jsonData = try encoder.encode(newLikeDislike)
        // ... and set our request's HTTP body
        request.httpBody = jsonData
        print("jsonData: ", String(data: request.httpBody!, encoding: .utf8) ?? "no body data")
    } catch {
        completion?(.failure(error))
    }
    
    // Create and run a URLSession data task with our JSON encoded POST request
    let config = URLSessionConfiguration.default
    let session = URLSession(configuration: config)
    let task = session.dataTask(with: request) { (responseData, response, responseError) in
        guard responseError == nil else {
            if let error = responseError {
                completion?(.failure(error))
            }
            return
        }
        
        // APIs usually respond with the data you just sent in your POST request
        if let data = responseData, let utf8Representation = String(data: data, encoding: .utf8) {
            print("response: ", utf8Representation)
            if let jsonData = responseData {
                // Now we have jsonData, Data representation of the JSON returned to us
                // from our URLRequest...
                
                // Create an instance of JSONDecoder to decode the JSON data to our
                // Codable struct
                let decoder = JSONDecoder()
                
                do {
                    // We would use Post.self for JSON representing a single Post
                    // object, and [Post].self for JSON representing an array of
                    // Post objects
                    let like = try decoder.decode(SubmitLikeDislike.self, from: jsonData)
                    completion?(.success(like))
                } catch {
                    completion?(.failure(error))
                }
            }
        } else {
            print("no readable data received in response")
        }
    }
    task.resume()
}
