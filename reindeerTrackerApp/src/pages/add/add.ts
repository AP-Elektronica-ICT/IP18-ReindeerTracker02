import { Component } from '@angular/core';
import { IonicPage, NavController, NavParams } from 'ionic-angular';
import { BarcodeScanner } from '@ionic-native/barcode-scanner';
import {ImagePicker} from "@ionic-native/image-picker";
import {AndroidPermissions} from "@ionic-native/android-permissions";
import {StorageProvider} from "../../providers/storage/storage";
import {ImageProvider} from "../../providers/image/image";
import {DeviceDetails} from "../../classes/deviceDetails";
import {DeviceProvider} from "../../providers/device/device";
import {PreloaderProvider} from "../../providers/preloader/preloader";

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
  imageUrl = null;
  imageChanged = false;
  deviceDetails: DeviceDetails = null;
  deviceKey = '';
  loaded = false;

  constructor(public navCtrl: NavController, public navParams: NavParams, private imagePicker: ImagePicker, private storage: StorageProvider, private image: ImageProvider, private deviceProvider: DeviceProvider, private preloader: PreloaderProvider) {
  }

  ionViewDidLoad() {
    this.deviceKey = this.navParams.get('deviceKey');
    this.deviceProvider.getDeviceDetails(this.deviceKey)
      .subscribe((details: DeviceDetails) => {
        this.deviceDetails = details;
        this.imageUrl = this.deviceDetails.imageUrl;
        this.loaded = true;
        console.log(details);
      }, err => {
        this.setDeviceDetailsEmpty();
        this.loaded = true;
      });
  }

  setDeviceDetailsEmpty() {
    this.deviceDetails = {
      name: '',
      birthyear: '',
      imageUrl: '',
      gender: 'M',
      activated: false
    };
  }

  getImage() {
    if (this.imageUrl) {
      return this.imageUrl;
    } else {
      return 'assets/imgs/no-image.png';
    }
  }

  getImageWithCamera() {
    this.image.selectImage()
      .then((result) => {
        this.imageUrl = result;
        this.imageChanged = true;
      })
  }

  base64toBlob(base64Data) {
    let sliceSize = 1024;
    let byteCharacters = atob(base64Data);
    let bytesLength = byteCharacters.length;
    let slicesCount = Math.ceil(bytesLength / sliceSize);
    let byteArrays = new Array(slicesCount);

    for (let sliceIndex = 0; sliceIndex < slicesCount; ++sliceIndex) {
      let begin = sliceIndex * sliceSize;
      let end = Math.min(begin + sliceSize, bytesLength);

      let bytes = new Array(end - begin);
      for (let offset = begin, i = 0 ; offset < end; ++i, ++offset) {
        bytes[i] = byteCharacters[offset].charCodeAt(0);
      }
      byteArrays[sliceIndex] = new Uint8Array(bytes);
    }
    return new Blob(byteArrays, { type: 'image/jpeg' });
  }

  submit(){
    if (this.imageChanged) {
      let b64data = this.imageUrl.split(',')[1];
      let blob = this.base64toBlob(b64data);
      this.storage.uploadImage(blob, this.deviceKey)
        .then(newImageUrl => {
          this.deviceDetails.imageUrl = newImageUrl;
          this.deviceProvider.setDeviceDetails(this.deviceKey, this.deviceDetails)
            .subscribe(res => {
              this.navCtrl.popAll();
            })
        })
        .catch(err => {
        })
    } else {
      console.log(this.deviceDetails);
      this.deviceProvider.setDeviceDetails(this.deviceKey, this.deviceDetails)
        .subscribe(res => {
          console.log(res);
          this.navCtrl.popAll();
        })
    }
  }

}
