import { Component } from '@angular/core';
import { IonicPage, NavController, NavParams } from 'ionic-angular';
import { BarcodeScanner } from '@ionic-native/barcode-scanner';

/**
 * Generated class for the AddPage page.
 *
 * See https://ionicframework.com/docs/components/#navigation for more info on
 * Ionic pages and navigation.
 */

@IonicPage()
@Component({
  selector: 'page-add',
  templateUrl: 'add.html',
})
export class AddPage {

  scannedCode = null;

  constructor(public navCtrl: NavController, public navParams: NavParams, private barcodeScanner : BarcodeScanner) {
  }

  ionViewDidLoad() {
    console.log('ionViewDidLoad AddPage');
  }

  scanQr(){
    this.barcodeScanner.scan().then(barcodeData => {
      this.scannedCode = barcodeData.text;
    })
  }

  submit(){

  }

}
