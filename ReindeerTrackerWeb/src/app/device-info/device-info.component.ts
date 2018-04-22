import {Component, OnInit, ViewChild} from '@angular/core';
import {DeviceService} from "../shared/device.service";
import {ActivatedRoute, Router} from "@angular/router";
import {Form, FormControl, FormGroup} from "@angular/forms";
import {DeviceDetails} from "../shared/device-details";
import {Location} from '@angular/common';
import {StorageService} from "../shared/storage.service";

@Component({
  selector: 'app-device-info',
  templateUrl: './device-info.component.html',
  styleUrls: ['./device-info.component.css']
})
export class DeviceInfoComponent implements OnInit {
  device: DeviceDetails;
  loaded = false;
  deviceKey: string;
  public detailForm: FormGroup;
  showChangesSavedMessage = false;
  showDeviceNotFoundMessage = false;
  imageFile = null;
  showActivateSuccessMessage = false;

  constructor(private deviceService: DeviceService, private route: ActivatedRoute, private location: Location, private storage: StorageService, private router: Router) { }

  ngOnInit() {
    this.showDeviceNotFoundMessage = false;
    this.route
      .queryParams
      .subscribe(params => {
        this.deviceKey = params.deviceKey;
        console.log(this.deviceKey);
        this.deviceService.getDeviceDetails(this.deviceKey)
          .subscribe((res: DeviceDetails) => {
            this.device = res;
            console.log(this.device);
            this.loaded = true;
            this.initForm();
          }, err => {
            this.showDeviceNotFoundMessage = true;
            this.loaded = true;
          })
      });
  }

  initForm() {
    let deviceGender = '';
    if (this.device.gender) {
      deviceGender = this.device.gender;
    }
    this.detailForm = new FormGroup({
      name: new FormControl(this.device.name),
      birthyear: new FormControl(this.device.birthyear),
      gender: new FormControl(deviceGender)
    })
  }

  getImageUrl(): string {
    if (this.device.imageUrl) {
      return this.device.imageUrl
    } else {
      return '../../assets/no-image.png';
    }
  }

  readUrl(event:any) {
    if (event.target.files && event.target.files[0]) {
      this.imageFile = event.target.files[0];
      var reader = new FileReader();

      reader.onload = (event:any) => {
        this.device.imageUrl = event.target.result;
      }

      reader.readAsDataURL(event.target.files[0]);
    }
  }

  onRemoveImage() {
    this.device.imageUrl = null;
  }

  onSubmit() {
    if (this.device.imageUrl) {
      this.storage.uploadImage(this.imageFile, this.deviceKey)
        .then(url => {
          this.device.imageUrl = url;
          this.updateDevice();
          this.deviceService.updateDeviceDetails(this.deviceKey, this.device)
            .subscribe(res => {
              this.deviceService.refreshUserDevices()
                .then(() => {
                  this.showChangesSavedMessage = true;
                  setTimeout(() => {
                    this.toHome();
                  }, 2000);
                })
            })
        });
    } else {
      this.updateDevice();
      this.deviceService.updateDeviceDetails(this.deviceKey, this.device)
        .subscribe(res => {
          this.deviceService.refreshUserDevices()
            .then(() => {
              if (this.detailForm.dirty) {
                this.showChangesSavedMessage = true;
                setTimeout(() => {
                  this.toHome();
                }, 2000);
              } else {
                this.toHome();
              }
            });
        })
    }
  }

  private toHome() {
    this.router.navigate(['/user']);
  }

  updateDevice() {
    this.device.name = this.detailForm.controls['name'].value;
    this.device.gender = this.detailForm.controls['gender'].value;
    this.device.birthyear = this.detailForm.controls['birthyear'].value;
  }

  onCancel() {
    this.location.back();
  }

  activateKey() {
    this.showActivateSuccessMessage = false;
    this.deviceService.activateDevice(this.deviceKey)
      .subscribe(res => {
        this.showActivateSuccessMessage = true;
        this.device.activated = true;
      }, err => {

      })
  }

}
