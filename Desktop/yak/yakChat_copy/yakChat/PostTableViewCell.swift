//
//  PostTableViewCell.swift
//  
//
//  Created by Seth Saperstein on 2/26/18.
//

import UIKit

class PostTableViewCell: UITableViewCell {

    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
    }

    override func setSelected(_ selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)

        // Configure the view for the selected state
    }

    @IBOutlet weak var postText: UILabel!
    
    @IBOutlet weak var time: UILabel!
    
    @IBOutlet weak var likes: UILabel!
    
    @IBOutlet weak var upVoteButton: UIButton!
    
    @IBOutlet weak var downVoteButton: UIButton!
    
}
