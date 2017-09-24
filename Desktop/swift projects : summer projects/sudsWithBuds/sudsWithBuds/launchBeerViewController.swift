//
//  launchBeerViewController.swift
//  sudsWithBuds
//
//  Created by Seth Saperstein on 9/24/17.
//  Copyright © 2017 Seth Saperstein. All rights reserved.
//

import UIKit

class launchBeerViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()

        //Beer.setBackgroundImage(UIImage(named: "Button.png"), for: .normal)
        // Do any additional setup after loading the view.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    var launchConnect : piConnect = piConnect()
    
    @IBAction func launchBeerButton(_ sender: UIButton) {
        launchConnect.switchon(1)
        launchConnect.switchon(2)
    }
    @IBAction func end(_ sender: UIButton) {
        launchConnect.switchon(3)
    }
    
    //@IBOutlet weak var Beer: UIButton!
    
    
    
    
    /*
    func switchon(_ on: Bool) {
        let mainsock: Int = Int(socket(AF_INET, SOCK_STREAM, 0))
        var server_address: sockaddr_in
        let host: hostent? = gethostbyname("35.0.62.171")
        //set host of our server
        memcpy(server_address.sin_addr.s_addr, host?.h_addr, host?.h_length)
        //set the host and length
        server_address.sin_family = sa_family_t(AF_INET)
        server_address.sin_port = htons(50007)
        //set port of our server
        if connect(mainsock, (server_address as? sockaddr), MemoryLayout<server_address>.size) < 0 {
            print("error connecting")
        }
        var returnval = "off"
        if on {
            returnval = "on"
        }
        
        let w: Int = write(Int32(mainsock), returnval.utf8CString, strlen(returnval.utf8CString))
    
        if w < 0 {
            print("error connecting")
        }
        close(Int32(mainsock))
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
