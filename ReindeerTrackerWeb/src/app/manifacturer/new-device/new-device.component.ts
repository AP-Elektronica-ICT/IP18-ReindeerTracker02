import { Component, OnInit } from '@angular/core';
import {FormGroup} from "@angular/forms";
import {KeysService} from "../../shared/keys.service";

@Component({
  selector: 'app-new-device',
  templateUrl: './new-device.component.html',
  styleUrls: ['./new-device.component.css'],
  providers: [KeysService]
})
export class NewDeviceComponent implements OnInit {

  newKeyList = [];
  errorMessage = '';
  showDevicesAddedMessage = false;
  deviceErrors = [];

  constructor(private keysService: KeysService) { }

  ngOnInit() {
  }

  onSubmitKey(keyForm: FormGroup) {
    const key = keyForm.controls.key.value.toString();
    if (!this.isKeyAlreadyAdded(key)) {
      if (this.checkKeyLength(key)) {
        this.newKeyList.push(key);
        keyForm.controls.key.setValue('');
        this.errorMessage = '';
      } else {
        this.errorMessage = 'Not a valid key.';
      }
    } else {
      this.errorMessage = 'This key already exists.'
    }
  }

  removeKey(index: number) {
    this.newKeyList.splice(index, 1);
  }

  isKeyAlreadyAdded(key: string): boolean {
    return this.newKeyList.indexOf(key) > -1;
  }

  checkKeyLength(key: string): boolean {
    return key.length === 6;
  }

  validateKeys() {
    this.showDevicesAddedMessage = false;
    this.keysService.addValidKeys(this.newKeyList)
      .subscribe((res) => {
        console.log(res);
        if (res.added.length === this.newKeyList.length) {
          this.showDevicesAddedMessage = true;
        } else {
          this.deviceErrors = res.errors;
        }
        this.newKeyList = [];
        this.errorMessage = '';
      })
  }
}
