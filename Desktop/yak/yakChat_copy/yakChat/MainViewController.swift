//
//  MainViewController.swift
//  yakChat
//
//  Created by Seth Saperstein on 2/26/18.
//  Copyright © 2018 Seth Saperstein. All rights reserved.
//

import UIKit
import SwiftHTTP

class MainViewController: UIViewController, UITableViewDataSource, UITableViewDelegate, NewPostViewControllerDelegate {
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        setupTableView()
        
        getTopPosts(type: feedType)
        
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    @IBOutlet weak var mainFeedTableView: FeedTableView!
    
    var recentPosts: RecentPosts? = nil
    
    var posts: [Post] = []
    
    var likes: [Likes] = []
    
    let uid = UIDevice.current.identifierForVendor?.uuidString
    
    var nexturl: String? = nil
    
    var feedType: String = "new" {
        didSet {
            getTopPosts(type: self.feedType)
        }
    }
    
    var segueToRow: Int? = nil
    
    private let refreshControl = UIRefreshControl()
    
    @IBAction func NewHotSegmentedControl(_ sender: UISegmentedControl) {
        switch sender.selectedSegmentIndex {
        case 0:
            // for new
            feedType = "new"
        case 1:
            // for hot
            feedType = "hot"
        default:
            fatalError("segment index type out of index")
        }
        
    }
    
    
    func didReturnFromNewPost(post: Post) {
        // self.posts.insert(post, at: 0)
        getTopPosts(type: feedType)
    }
    
    func setupTableView() {
        mainFeedTableView.delegate = self
        mainFeedTableView.dataSource = self
        
        // Add Refresh Control to Table View
        if #available(iOS 10.0, *) {
            mainFeedTableView.refreshControl = refreshControl
        } else {
            mainFeedTableView.addSubview(refreshControl)
        }
        // Configure Refresh Control
        refreshControl.addTarget(self, action: #selector(refreshPosts(_:)), for: .valueChanged)
        refreshControl.attributedTitle = NSAttributedString(string: "Fetching New Yaks ...")
    }
    
    @objc private func refreshPosts(_ sender: Any) {
        getTopPosts(type: self.feedType)
    }
    
    func getTopPosts(type: String) {
        self.posts = []
        self.likes = []
        getRecentPosts(type: type) { (result) in
            switch result {
            case .success(let recPosts):
                print("get recent posts success")
                self.recentPosts = recPosts
                for post in (self.recentPosts?.results)! {
                    let postid = String(post.postid)
                    getPosts(for: postid) { (result2) in
                        switch result2 {
                        case .success(let post):
                            print("got post \(postid)")
                            self.posts.append(post)
                            
                            
                        case .failure(let error2):
                            fatalError("error: \(error2.localizedDescription)")
                        }
                    }
                    getLikeInfo(postid: postid){ (result3) in
                        switch result3 {
                        case .success(let likeInfo):
                            print("getting like for post \(postid)")
                            let curPostLike = Likes(logname_likes_this: likeInfo.logname_likes_this, logname_dislikes_this: likeInfo.logname_dislikes_this, net_likes: likeInfo.net_likes, postid: likeInfo.postid)
                           self.likes.append(curPostLike)
                           
                            if self.likes.count == recPosts.results.count && self.likes.count == self.posts.count {
                                DispatchQueue.main.async(execute: {() -> Void in
                                    self.mainFeedTableView.reloadData()
                                })
                            }
                
                        case .failure(let error3):
                            fatalError("error: \(error3.localizedDescription)")
                        }
                        
                    }
                }
            case .failure(let error):
                fatalError("error: \(error.localizedDescription)")
            }
            DispatchQueue.main.async(execute: {() -> Void in
                self.refreshControl.endRefreshing()
            })
        }
    }
    
    @objc func upVoteClicked(sender:UIButton) {
        let buttonRow = sender.tag
        
        let postid = String(self.likes[buttonRow].postid)
        
        if (!(self.likes[buttonRow].logname_likes_this)) {
            // if we need to delete the downvote if exists
            if (self.likes[buttonRow].logname_dislikes_this) {
                deleteLikeDislike(postid: postid, like: false, dislike: true) { (result) in
                    switch result {
                    case .success(let data):
                       print(data)
                       self.likes[buttonRow].logname_dislikes_this = false
                        self.likes[buttonRow].net_likes += 1
                       DispatchQueue.main.async(execute: {() -> Void in
                        self.mainFeedTableView.reloadData()
                       })
                    case .failure(let error):
                        fatalError("error: \(error.localizedDescription)")
                    }
                    
                }
            }
            
            submitLikeDislike(postid: postid, like: true, dislike: false) { (result) in
                switch result {
                case .success(let data):
                   
                   print(data)
                   self.likes[buttonRow].logname_likes_this = true
                    self.likes[buttonRow].net_likes += 1
                    
                    if self.likes[buttonRow].net_likes <= -5 {
                        self.likes.remove(at: buttonRow)
                        self.posts.remove(at: buttonRow)
                    }
                    
                    DispatchQueue.main.async(execute: {() -> Void in
                        self.mainFeedTableView.reloadData()
                    })
                case .failure(let error):
                    fatalError("error: \(error.localizedDescription)")
                }
            }
        }
        // if user liked this and is now unliking
        else {
            deleteLikeDislike(postid: postid, like: true, dislike: false) { (result) in
                switch result {
                case .success(let data):
                    print(data)
                    self.likes[buttonRow].logname_likes_this = false
                    self.likes[buttonRow].net_likes -= 1
                    DispatchQueue.main.async(execute: {() -> Void in
                        self.mainFeedTableView.reloadData()
                    })
                case .failure(let error):
                    fatalError("error: \(error.localizedDescription)")
                }
                
            }
        }
    }
    
    @objc func downVoteClicked(sender:UIButton) {
        let buttonRow = sender.tag
        
        let postid = String(self.likes[buttonRow].postid)
        if (!(self.likes[buttonRow].logname_dislikes_this)) {
            // if we need to delete the upvote if it exists
            if (self.likes[buttonRow].logname_likes_this) {
                deleteLikeDislike(postid: postid, like: true, dislike: false) { (result) in
                    switch result {
                    case .success(let data):
                       
                        print(data)
                        self.likes[buttonRow].logname_likes_this = false
                        self.likes[buttonRow].net_likes -= 1
                        DispatchQueue.main.async(execute: {() -> Void in
                            self.mainFeedTableView.reloadData()
                        })
                    case .failure(let error):
                        fatalError("error: \(error.localizedDescription)")
                    }
                }
            }
            
            submitLikeDislike(postid: postid, like: false, dislike: true) { (result) in
                switch result {
                case .success(let data):
                   print(data)
                   self.likes[buttonRow].logname_dislikes_this = true
                    self.likes[buttonRow].net_likes -= 1
                    
                    if self.likes[buttonRow].net_likes <= -5 {
                        self.likes.remove(at: buttonRow)
                        self.posts.remove(at: buttonRow)
                    }
                    
                    DispatchQueue.main.async(execute: {() -> Void in
                        self.mainFeedTableView.reloadData()
                    })
                case .failure(let error):
                    fatalError("error: \(error.localizedDescription)")
                }
            }
        }
        // if user disliked this and is now undisliking
        else {
            deleteLikeDislike(postid: postid, like: false, dislike: true) { (result) in
                switch result {
                case .success(let data):
                    print(data)
                    self.likes[buttonRow].logname_dislikes_this = false
                    self.likes[buttonRow].net_likes += 1
                    DispatchQueue.main.async(execute: {() -> Void in
                        self.mainFeedTableView.reloadData()
                    })
                case .failure(let error):
                    fatalError("error: \(error.localizedDescription)")
                }
                
            }
        }
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        if segue.identifier == "toCreatePost" {
            if let nc = segue.destination as? UINavigationController {
                if let vc = nc.topViewController as? NewPostViewController {
                    vc.delegate = self
                }
            }
        }
        else if segue.identifier == "toComments" {
            if let nc = segue.destination as? UINavigationController {
                if let vc = nc.topViewController as? CommentViewController {
                    vc.mainPost = posts[self.segueToRow!]
                    vc.mainPostLikes = likes[self.segueToRow!]
                }
            }
        }
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "cell", for: indexPath) as! PostTableViewCell
        let row = indexPath.row
        cell.postText.text = (posts[row]).text
        cell.time.text = (posts[row]).age
        cell.upVoteButton.tag = indexPath.row
        cell.downVoteButton.tag = indexPath.row
        let liked = self.likes[row].logname_likes_this
        let disliked = self.likes[row].logname_dislikes_this
        if liked {
            cell.upVoteButton.titleLabel?.textColor = UIColor.blue
            cell.downVoteButton.titleLabel?.textColor = UIColor.gray
        }
        else if disliked {
            cell.downVoteButton.titleLabel?.textColor = UIColor.blue
            cell.upVoteButton.titleLabel?.textColor = UIColor.gray
        }
        else {
            cell.downVoteButton.titleLabel?.textColor = UIColor.gray
            cell.downVoteButton.titleLabel?.textColor = UIColor.gray
        }
        cell.likes.text = String(self.likes[row].net_likes)
        cell.upVoteButton.addTarget(self, action: #selector(MainViewController.upVoteClicked(sender:)), for: UIControlEvents.touchUpInside)
        cell.downVoteButton.addTarget(self, action: #selector(MainViewController.downVoteClicked(sender:)), for: UIControlEvents.touchUpInside)
        
        return cell
    }
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return posts.count
    }
    
    func tableView(_ tableView: UITableView, willDisplay cell: UITableViewCell, forRowAt indexPath: IndexPath) {
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        self.segueToRow = indexPath.row
        performSegue(withIdentifier: "toComments", sender: self)
    }
    
    

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */

}

