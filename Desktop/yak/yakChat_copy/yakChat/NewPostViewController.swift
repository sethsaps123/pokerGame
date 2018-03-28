//
//  NewPostViewController.swift
//  yakChat
//
//  Created by Seth Saperstein on 3/2/18.
//  Copyright © 2018 Seth Saperstein. All rights reserved.
//

import UIKit

protocol NewPostViewControllerDelegate {
    func didReturnFromNewPost(post: Post)
}

class NewPostViewController: UIViewController, UITextFieldDelegate {

    override func viewDidLoad() {
        super.viewDidLoad()
        postTextField.becomeFirstResponder()
        postTextField.delegate = self
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    var delegate: NewPostViewControllerDelegate?
    
    @IBOutlet weak var characterCountLabel: UILabel!
    
    @IBOutlet weak var postTextField: UITextField!
  
    @IBAction func textFieldEditingChanged(_ sender: UITextField) {
        characterCountLabel.text = String((240 - (sender.text?.count)!))
    }
    
    func textField(_ textField: UITextField, shouldChangeCharactersIn range: NSRange, replacementString string: String) -> Bool {
        let maxLength = 240
        let currentString: NSString = textField.text! as NSString
        let newString: NSString =
            currentString.replacingCharacters(in: range, with: string) as NSString
        return newString.length <= maxLength
    }
    
    @IBAction func createPost(_ sender: UIBarButtonItem) {
        let myPost = NewPost(text: postTextField.text!)
        
        submitPost(post: myPost) { (result) in
            switch result {
            case .success(let newPost)?:
                self.delegate?.didReturnFromNewPost(post: newPost)
            case .failure(let error)?:
                fatalError("error: \(error.localizedDescription)")
            case .none:
                fatalError("error: none case for create post")
            }
        }
        
        self.dismiss(animated: true)
        
    }
    
    @IBAction func cancelNewPostButton(_ sender: UIBarButtonItem) {
        self.dismiss(animated: true)
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
