import { Component, OnInit } from '@angular/core';
import {AuthService} from "../shared/auth.service";
import {FormGroup} from "@angular/forms";
import {KeysService} from "../shared/keys.service";
import {DeviceService} from "../shared/device.service";

@Component({
  selector: 'app-add-device',
  templateUrl: './add-device.component.html',
  styleUrls: ['./add-device.component.css'],
  providers: []
})
export class AddDeviceComponent implements OnInit {
  key = '';
  errorMessage = '';
  showSuccessMessage = false;
  showActivateSuccessMessage = false;

  constructor(private auth: AuthService, private keyService: KeysService, private deviceService: DeviceService) { }

  ngOnInit() {

  }

  onSubmitKey(keyForm: FormGroup) {
    this.key = keyForm.controls.key.value.toString();
    if (this.keyService.checkKeyLength(this.key)) {
      this.sendKey(this.key);
      this.errorMessage = '';
    } else {
      this.errorMessage = 'Not a valid key.';
    }
  }

  private sendKey(key: string) {
    this.showSuccessMessage = false;
    this.showActivateSuccessMessage = false;
    this.errorMessage = '';
    this.keyService.addKeyToUser(this.auth.getCurrentUID(), key)
      .subscribe(res => {
        console.log(res);
        this.showSuccessMessage = true;
      },
        err => {
          console.log(err);
          if (err.status === 404) {
            this.errorMessage = 'A device with the key ' + key + ' does not exist';
          }
        });
  }

  activateKey() {
    this.showSuccessMessage = false;
    this.deviceService.activateDevice(this.key)
      .subscribe(res => {
        this.showActivateSuccessMessage = true;
      }, err => {

      })
  }

  dissmissActivation() {
    this.showSuccessMessage = false;
  }

}
