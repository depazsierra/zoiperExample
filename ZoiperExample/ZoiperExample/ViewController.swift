//
//  ViewController.swift
//  ZoiperExample
//
//  Created by Diego de Paz Sierra on 7/7/16.
//  Copyright © 2016 depa. All rights reserved.
//

import UIKit

class ViewController: UIViewController {
    
    @IBOutlet weak var telNumber: UITextField!
    
    @IBOutlet weak var label: UILabel!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        ZoiperVoip.sharedInstance().setupSIP();
        ZoiperVoip.sharedInstance().activationRegister("", password: "")
        NotificationCenter.default.addObserver(self, selector: #selector(ViewController.ctxRegistrationSucceeded(_:)), name: NSNotification.Name(rawValue: "ZSDKctxDidRegistrationSucceeded"), object: nil)
        
        NotificationCenter.default.addObserver(self, selector: #selector(ViewController.ctxActivationStatusUpdated(_:)), name: NSNotification.Name(rawValue: "ZSDKctxDidActivationStatusUpdated"), object: nil)
        
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    @IBAction func registrarAction(_ sender: AnyObject) {
        
        ZoiperVoip.sharedInstance().registerSIP(
            withUser: "<user>", pass: "<pass>", server: "<server>", proxy: "<proxy>")
    }
    
    @IBAction func llamarAction(_ sender: AnyObject) {
        ZoiperVoip.sharedInstance().callNumber(telNumber.text)
    }
    
    func ctxRegistrationSucceeded(_ notification:Notification) {
        self.label.text = "Register User OK"
    }
    
    func ctxActivationStatusUpdated(_ notification:Notification) {
        self.label.text = "Activation OK"
    }
    
    @IBAction func colgarAction(_ sender: AnyObject) {
        ZoiperVoip.sharedInstance().callHangout()
    }
}

