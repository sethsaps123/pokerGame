//
//  friendList.swift
//  sudsWithBuds
//
//  Created by Seth Saperstein on 9/23/17.
//  Copyright © 2017 Seth Saperstein. All rights reserved.
//

import UIKit

class friendList: UITableViewController {
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.tableView.backgroundView = UIImageView(image: UIImage(named: "bubbles2.png"))

    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    // MARK: - Table view data source

    override func numberOfSections(in tableView: UITableView) -> Int {
        // #warning Incomplete implementation, return the number of sections
        return 1
    }

    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        // #warning Incomplete implementation, return the number of rows
        return 10
    }

    
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        // Configure the cell...
        
        let dequeued = tableView.dequeueReusableCell(withIdentifier: "drinkerCell", for: indexPath)
        let cell = dequeued as? oneDrinkerCellTableViewCell
        
        //update this part with actual data later
        
        let drinkName = drinks[indexPath.row]
        let friendName = friends[indexPath.row]
        let time = times[indexPath.row]
        cell?.infoShownByCell = [friendName, drinkName, time]
        cell?.DrinkImage.image = UIImage(named: drinkName + ".png")
        cell?.backgroundColor = .clear
        return cell!
    }
    
    var drinks : [String] = ["Monster Energy", "Coca Cola", "Sprite",
        "Pabst Blue Ribbon", "Red Stripe", "Budweiser", "Heinekin",
        "Mountain Dew", "Diet Coke", "Red Bull"]
    var friends : [String] = ["Seth Saperstein", "Eris Llangos",
                              "Shivam Shwarma", "Derek", "Addison Viener",
                              "Austin Avery", "Jack Leitner", "Asher Bond",
                              "Alasgar", "Matt Kapur"]
    var times : [String] = ["6:00 am", "4:45 am", "2:00 am", "1:32 am",
                            "12:00 am", "11:30 pm", "11:29 pm", "10:30 pm",
                            "9:00 pm", "17:38 bc"]
    
    
    
    

    /*
    // Override to support conditional editing of the table view.
    override func tableView(_ tableView: UITableView, canEditRowAt indexPath: IndexPath) -> Bool {
        // Return false if you do not want the specified item to be editable.
        return true
    }
    */

    /*
    // Override to support editing the table view.
    override func tableView(_ tableView: UITableView, commit editingStyle: UITableViewCellEditingStyle, forRowAt indexPath: IndexPath) {
        if editingStyle == .delete {
            // Delete the row from the data source
            tableView.deleteRows(at: [indexPath], with: .fade)
        } else if editingStyle == .insert {
            // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
        }    
    }
    */

    /*
    // Override to support rearranging the table view.
    override func tableView(_ tableView: UITableView, moveRowAt fromIndexPath: IndexPath, to: IndexPath) {

    }
    */

    /*
    // Override to support conditional rearranging of the table view.
    override func tableView(_ tableView: UITableView, canMoveRowAt indexPath: IndexPath) -> Bool {
        // Return false if you do not want the item to be re-orderable.
        return true
    }
    */

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */

}
