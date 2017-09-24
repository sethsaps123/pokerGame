//
//  beerScanningViewController.swift
//  sudsWithBuds
//
//  Created by Seth Saperstein on 9/24/17.
//  Copyright © 2017 Seth Saperstein. All rights reserved.
//

import UIKit


class beerScanningViewController: UIViewController, takingPhotoViewControllerDelegate {

    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        if segue.identifier == "toCamera" {
            let nav = segue.destination as! takingPhotoViewController
            nav.delegate = self
        }
    }
    
    func giveCode(code: String) {
        currentCode = code
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
    }


    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    var currentCode : String? {
        didSet {
            Detection.text = barcodeLookup[currentCode!]
            beerImage.image = UIImage(named: Detection.text! + ".png")
            comment.text = "Drink successfully added!"
        }
    }
    
    @IBOutlet weak var comment: UILabel!
    @IBOutlet weak var Detection: UILabel!
    @IBOutlet weak var beerImage: UIImageView!
    
    
    var barcodeLookup: [String : String] = [
        "0070847811268" : "Monster Energy",
        "0611269991000" : "Red Bull",
        "049000001327" : "Sprite",
        "012000809965" : "Mountain Dew",
        "049000429183" : "Diet Coke",
        "049000608779" : "Coca Cola"
    ]
    

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */

}
