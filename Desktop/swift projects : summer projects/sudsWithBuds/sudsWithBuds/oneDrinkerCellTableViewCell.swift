//
//  oneDrinkerCellTableViewCell.swift
//  sudsWithBuds
//
//  Created by Seth Saperstein on 9/23/17.
//  Copyright © 2017 Seth Saperstein. All rights reserved.
//

import UIKit

class oneDrinkerCellTableViewCell: UITableViewCell {

    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
    }

    override func setSelected(_ selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)

        // Configure the view for the selected state
    }

    @IBOutlet weak var Name: UILabel!
    @IBOutlet weak var Drink: UILabel!
    @IBOutlet weak var Time: UILabel!
    @IBOutlet weak var DrinkImage: UIImageView!
    
    var infoShownByCell: [String]? {
        didSet {
            Name.text = infoShownByCell?[0]
            Drink.text = infoShownByCell?[1]
            Time.text = infoShownByCell?[2]
            
            Time.textColor = UIColor.lightGray
            
        }
    }
    
}
