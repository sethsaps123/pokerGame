//
//  CommentViewController.swift
//  yakChat
//
//  Created by Seth Saperstein on 3/26/18.
//  Copyright © 2018 Seth Saperstein. All rights reserved.
//

import UIKit

class CommentViewController: UIViewController, UITableViewDataSource, UITableViewDelegate, UITextFieldDelegate {
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        setupTableView()
        
        getComments(posturl: (mainPost?.url)!)
        
        newCommentTextField.delegate = self
        
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    @IBOutlet weak var commentsTableView: CommentTableView!
    
    var mainPost: Post? = nil
    
    var mainPostLikes: Likes? = nil
    
    var recentPosts: RecentPosts? = nil
    
    var comments: [Post] = []
    
    var likes: [Likes] = []
    
    let uid = UIDevice.current.identifierForVendor?.uuidString
    
    var nexturl: String? = nil
    
    
    
    private let refreshControl = UIRefreshControl()
    
    func setupTableView() {
        commentsTableView.delegate = self
        commentsTableView.dataSource = self
        
        // Add Refresh Control to Table View
        if #available(iOS 10.0, *) {
            commentsTableView.refreshControl = refreshControl
        } else {
            commentsTableView.addSubview(refreshControl)
        }
        // Configure Refresh Control
        refreshControl.addTarget(self, action: #selector(refreshPosts(_:)), for: .valueChanged)
        refreshControl.attributedTitle = NSAttributedString(string: "Fetching New Yaks ...")
    }
    
    func textField(_ textField: UITextField, shouldChangeCharactersIn range: NSRange, replacementString string: String) -> Bool {
        let maxLength = 120
        let currentString: NSString = textField.text! as NSString
        let newString: NSString =
            currentString.replacingCharacters(in: range, with: string) as NSString
        return newString.length <= maxLength
    }
    
    @objc private func refreshPosts(_ sender: Any) {
        getComments(posturl: (mainPost?.url)!)
    }
    
    @IBAction func newCommentButton(_ sender: UIButton) {
        let comment = NewPost(text: newCommentTextField.text!)
        submitComment(comment: comment, posturl: (mainPost?.url)!) { (result) in
            switch result {
            case .success(let newPost)?:
                DispatchQueue.main.async(execute: {() -> Void in
                    self.newCommentTextField?.text = ""
                })
                self.getComments(posturl: (self.mainPost?.url)!)
                print(newPost)
            case .failure(let error)?:
                fatalError("error: \(error.localizedDescription)")
            case .none:
                fatalError("error: none case for create post")
            }
        }
    }
    
    @IBAction func backBarButton(_ sender: UIBarButtonItem) {
        self.dismiss(animated: true)
    }
    
    
    @IBOutlet weak var newCommentTextField: UITextField!
    
    
    func getComments(posturl: String) {
        self.comments = []
        getAllComments(posturl: (mainPost?.url)!) { (result) in
            switch result {
            case .success(let recPosts):
                print("get recent posts success")
                self.recentPosts = recPosts
                for comment in (self.recentPosts?.results)! {
                    let commentid = String(comment.postid)
                    getComment(for: self.mainPost!.url, for: commentid) { (result2) in
                        switch result2 {
                        case .success(let comment):
                            print("getPosts success")
                            self.comments.append(comment)
                            print("appended a post")
                            
                            
                        case .failure(let error2):
                            fatalError("error: \(error2.localizedDescription)")
                        }
                    }
                    getCommentLikeInfo(posturl: self.mainPost!.url, commentid: commentid){ (result3) in
                        print("getting like info")
                        switch result3 {
                        case .success(let likeInfo):
                            
                            let curPostLike = Likes(logname_likes_this: likeInfo.logname_likes_this, logname_dislikes_this: likeInfo.logname_dislikes_this, net_likes: likeInfo.net_likes, postid: likeInfo.postid)
                            self.likes.append(curPostLike)
                            if (recPosts.results.count == self.likes.count && self.likes.count == self.comments.count) {
                                DispatchQueue.main.async(execute: {() -> Void in
                                    self.commentsTableView.reloadData()
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
        
        let postid = String(self.likes[buttonRow - 1].postid)
        
        if (!(self.likes[buttonRow - 1].logname_likes_this)) {
            // if we need to delete the downvote if exists
            if (self.likes[buttonRow - 1].logname_dislikes_this) {
                deleteCommentLikeDislike(posturl: (mainPost?.url)!, commentid: postid,  like: false, dislike: true) { (result) in
                    switch result {
                    case .success(let data):
                        print(data)
                        self.likes[buttonRow - 1].logname_dislikes_this = false
                        self.likes[buttonRow - 1].net_likes += 1
                        DispatchQueue.main.async(execute: {() -> Void in
                            self.commentsTableView.reloadData()
                        })
                    case .failure(let error):
                        fatalError("error: \(error.localizedDescription)")
                    }
                    
                }
            }
            
            submitCommentLikeDislike(posturl: (mainPost?.url)!, commentid: postid, like: true, dislike: false) { (result) in
                switch result {
                case .success(let data):
                    
                    print(data)
                    self.likes[buttonRow - 1].logname_likes_this = true
                    self.likes[buttonRow - 1].net_likes += 1
                    
                    if self.likes[buttonRow - 1].net_likes <= -5 {
                        self.likes.remove(at: buttonRow - 1)
                        self.comments.remove(at: buttonRow - 1)
                    }
                    
                    DispatchQueue.main.async(execute: {() -> Void in
                        self.commentsTableView.reloadData()
                    })
                case .failure(let error):
                    fatalError("error: \(error.localizedDescription)")
                }
            }
        }
            // if user liked this and is now unliking
        else {
            deleteCommentLikeDislike(posturl: (mainPost?.url)!, commentid: postid, like: true, dislike: false) { (result) in
                switch result {
                case .success(let data):
                    print(data)
                    self.likes[buttonRow - 1].logname_likes_this = false
                    self.likes[buttonRow - 1].net_likes -= 1
                    DispatchQueue.main.async(execute: {() -> Void in
                        self.commentsTableView.reloadData()
                    })
                case .failure(let error):
                    fatalError("error: \(error.localizedDescription)")
                }
                
            }
        }
    }
    
    @objc func downVoteClicked(sender:UIButton) {
        let buttonRow = sender.tag
        
        let postid = String(self.likes[buttonRow - 1].postid)
        if (!(self.likes[buttonRow - 1].logname_dislikes_this)) {
            // if we need to delete the upvote if it exists
            if (self.likes[buttonRow - 1].logname_likes_this) {
                deleteCommentLikeDislike(posturl: (mainPost?.url)!, commentid: postid, like: true, dislike: false) { (result) in
                    switch result {
                    case .success(let data):
                        
                        print(data)
                        self.likes[buttonRow - 1].logname_likes_this = false
                        self.likes[buttonRow - 1].net_likes -= 1
                        DispatchQueue.main.async(execute: {() -> Void in
                            self.commentsTableView.reloadData()
                        })
                    case .failure(let error):
                        fatalError("error: \(error.localizedDescription)")
                    }
                }
            }
            
            submitCommentLikeDislike(posturl: (mainPost?.url)!, commentid: postid, like: false, dislike: true) { (result) in
                switch result {
                case .success(let data):
                    print(data)
                    self.likes[buttonRow - 1].logname_dislikes_this = true
                    self.likes[buttonRow - 1].net_likes -= 1
                    
                    if self.likes[buttonRow - 1].net_likes <= -5 {
                        self.likes.remove(at: buttonRow - 1)
                        self.comments.remove(at: buttonRow - 1)
                    }
                    
                    DispatchQueue.main.async(execute: {() -> Void in
                        self.commentsTableView.reloadData()
                    })
                case .failure(let error):
                    fatalError("error: \(error.localizedDescription)")
                }
            }
        }
            // if user disliked this and is now undisliking
        else {
            deleteCommentLikeDislike(posturl: (mainPost?.url)!, commentid: postid, like: false, dislike: true) { (result) in
                switch result {
                case .success(let data):
                    print(data)
                    self.likes[buttonRow - 1].logname_dislikes_this = false
                    self.likes[buttonRow - 1].net_likes += 1
                    DispatchQueue.main.async(execute: {() -> Void in
                        self.commentsTableView.reloadData()
                    })
                case .failure(let error):
                    fatalError("error: \(error.localizedDescription)")
                }
                
            }
        }
    }
    
    @objc func mainUpVoteClicked(sender:UIButton) {
        
        let postid = String(describing: (self.mainPostLikes?.postid)!)
        
        if (!((self.mainPostLikes?.logname_likes_this)!)) {
            // if we need to delete the downvote if exists
            if (self.mainPostLikes?.logname_dislikes_this)! {
                deleteLikeDislike(postid: postid,  like: false, dislike: true) { (result) in
                    switch result {
                    case .success(let data):
                        print(data)
                        self.mainPostLikes?.logname_dislikes_this = false
                        self.mainPostLikes?.net_likes += 1
                        DispatchQueue.main.async(execute: {() -> Void in
                            self.commentsTableView.reloadData()
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
                    self.mainPostLikes?.logname_likes_this = true
                    self.mainPostLikes?.net_likes += 1
                    /*
                    if (self.mainPostLikes?.net_likes)! <= -5 {
                     
                        self.likes.remove(at: buttonRow)
                        self.comments.remove(at: buttonRow)
                    }
                    */
                    DispatchQueue.main.async(execute: {() -> Void in
                        self.commentsTableView.reloadData()
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
                    self.mainPostLikes?.logname_likes_this = false
                    self.mainPostLikes?.net_likes -= 1
                    DispatchQueue.main.async(execute: {() -> Void in
                        self.commentsTableView.reloadData()
                    })
                case .failure(let error):
                    fatalError("error: \(error.localizedDescription)")
                }
                
            }
        }
    }
    
    @objc func mainDownVoteClicked(sender:UIButton) {
        
        let postid = String(describing: (self.mainPostLikes?.postid)!)
        if (!((self.mainPostLikes?.logname_dislikes_this)!)) {
            // if we need to delete the upvote if it exists
            if (self.mainPostLikes?.logname_likes_this)! {
                deleteLikeDislike(postid: postid, like: true, dislike: false) { (result) in
                    switch result {
                    case .success(let data):
                        
                        print(data)
                        self.mainPostLikes?.logname_likes_this = false
                        self.mainPostLikes?.net_likes -= 1
                        DispatchQueue.main.async(execute: {() -> Void in
                            self.commentsTableView.reloadData()
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
                    self.mainPostLikes?.logname_dislikes_this = true
                    self.mainPostLikes?.net_likes -= 1
                    /*
                    if self.mainPostLikes?.net_likes <= -5 {
                        self.likes.remove(at: buttonRow)
                        self.comments.remove(at: buttonRow)
                    }
                    */
                    DispatchQueue.main.async(execute: {() -> Void in
                        self.commentsTableView.reloadData()
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
                    self.mainPostLikes?.logname_dislikes_this = false
                    self.mainPostLikes?.net_likes += 1
                    DispatchQueue.main.async(execute: {() -> Void in
                        self.commentsTableView.reloadData()
                    })
                case .failure(let error):
                    fatalError("error: \(error.localizedDescription)")
                }
                
            }
        }
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        var cell = tableView.dequeueReusableCell(withIdentifier: "cell", for: indexPath) as! PostTableViewCell
        let row = indexPath.row
        if row == 0 {
            cell = tableView.dequeueReusableCell(withIdentifier: "mainCell", for: indexPath) as! PostTableViewCell
            cell.postText.text = mainPost?.text
            cell.time.text = mainPost?.age
            cell.upVoteButton.tag = indexPath.row
            cell.downVoteButton.tag = indexPath.row
            let liked = self.mainPostLikes?.logname_likes_this
            let disliked = self.mainPostLikes?.logname_dislikes_this
            if liked! {
                cell.upVoteButton.titleLabel?.textColor = UIColor.blue
                cell.downVoteButton.titleLabel?.textColor = UIColor.gray
            }
            else if disliked! {
                cell.downVoteButton.titleLabel?.textColor = UIColor.blue
                cell.upVoteButton.titleLabel?.textColor = UIColor.gray
            }
            else {
                cell.downVoteButton.titleLabel?.textColor = UIColor.gray
                cell.downVoteButton.titleLabel?.textColor = UIColor.gray
            }
            cell.likes.text = String(describing: (self.mainPostLikes?.net_likes)!)
            cell.upVoteButton.addTarget(self, action: #selector(CommentViewController.mainUpVoteClicked(sender:)), for: UIControlEvents.touchUpInside)
            cell.downVoteButton.addTarget(self, action: #selector(CommentViewController.mainDownVoteClicked(sender:)), for: UIControlEvents.touchUpInside)
            return cell
            
        }
        cell.postText.text = (comments[row - 1]).text
        cell.time.text = (comments[row - 1]).age
        cell.upVoteButton.tag = row
        cell.downVoteButton.tag = row
        let liked = self.likes[row - 1].logname_likes_this
        let disliked = self.likes[row - 1].logname_dislikes_this
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
        cell.likes.text = String(self.likes[row - 1].net_likes)
        cell.upVoteButton.addTarget(self, action: #selector(CommentViewController.upVoteClicked(sender:)), for: UIControlEvents.touchUpInside)
        cell.downVoteButton.addTarget(self, action: #selector(CommentViewController.downVoteClicked(sender:)), for: UIControlEvents.touchUpInside)
        
        return cell
    }
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return comments.count + 1
    }
    
    func tableView(_ tableView: UITableView, willDisplay cell: UITableViewCell, forRowAt indexPath: IndexPath) {
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

