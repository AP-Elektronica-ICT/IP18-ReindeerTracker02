import { Component, OnInit } from '@angular/core';
import {AuthService} from "../shared/auth.service";
import {Router} from "@angular/router";
import {DeviceService} from "../shared/device.service";

@Component({
  selector: 'app-navbar',
  templateUrl: './navbar.component.html',
  styleUrls: ['./navbar.component.css']
})
export class NavbarComponent implements OnInit {

  constructor(public authService: AuthService, private router: Router, private deviceService: DeviceService) { }

  ngOnInit() {

  }

  signOut() {
    this.authService.signOut()
      .then(res => {
        this.deviceService.resetDevices();
        this.router.navigate(['/']);
      })
  }
}
