import { Component, OnInit } from '@angular/core';
import {AuthService} from "../shared/auth.service";
import {Userdata} from "../shared/userdata";

@Component({
  selector: 'app-profile',
  templateUrl: './profile.component.html',
  styleUrls: ['./profile.component.css']
})
export class ProfileComponent implements OnInit {
  currentUser: Userdata = null;

  constructor(private authService: AuthService) {
    this.currentUser = this.authService.getCurrentUser();
  }

  ngOnInit() {
  }

}
