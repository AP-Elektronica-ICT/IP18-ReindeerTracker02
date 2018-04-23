import { Component, OnInit } from '@angular/core';
import {DeviceService} from "../shared/device.service";
import {ActivatedRoute} from "@angular/router";

@Component({
  selector: 'app-detail',
  templateUrl: './detail.component.html',
  styleUrls: ['./detail.component.css']
})
export class DetailComponent implements OnInit {
// AGM static -----
lat: number = 51.167711;
lng: number = 4.070101;
rad: number = 600;
col: string = 'green';
op: number = 0.3;
mapType: string = 'hybrid';

ft(name:string){
  this.mapType = name;
  console.log(name);
}
// AGM static -----
  device = null;

  constructor(private deviceService: DeviceService, private route: ActivatedRoute) {
    this.route.queryParams
      .subscribe(params => {
        this.getDeviceDetails(params.deviceKey);
      })
  }

  getDeviceDetails(deviceKey: string) {
    this.deviceService.getDeviceFull(deviceKey)
      .subscribe(res => {
        this.device = res;
        console.log(res)
      })
      }

  getAliveStatus() {
    if (this.device.isAlive) {
      return 'alive';
    } else {
      return 'dead';
    }
  }

  ngOnInit() {

  }

}
