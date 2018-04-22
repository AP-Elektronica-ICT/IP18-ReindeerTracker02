import { Component, OnInit } from '@angular/core';
import {Userdata} from "../shared/userdata";
import {AuthService} from "../shared/auth.service";
import {Router} from "@angular/router";
import {Location} from '@angular/common';

@Component({
  selector: 'app-profile-edit',
  templateUrl: './profile-edit.component.html',
  styleUrls: ['./profile-edit.component.css']
})
export class ProfileEditComponent implements OnInit {
  locationArr = ["Käsivarsi", "Kemin-Sompio", "Kiiminki", "Kolari", "Kollaja", "Kuivasalmi", "Kuukas", "Kyrö", "Lappi", "Lohijärvi", "Muddusjärvi", "Muonio", "Muotkatunturi", "Näätämö", "Näkkälä", "Näljänkä", "Narkaus", "Niemelä", "Oijärvi", "Oivanki", "Orajärvi", "Oraniemi", "Paatsjoki", "Paistunturi", "Palojärvi", "Pintamo", "Pohjois-Salla", "Poikajärvi", "Posion", "Livo", "Pudasjärven", "Livo", "Pudasjärvi", "Pyhä-Kallio", "Salla", "Sallivaara", "Sattasniemi", "Syväjärvi", "Taivalkoski", "Timisjärvi", "Tolva", "Vanttaus", "Vätsäri"]
  currentUser: Userdata = null;

  constructor(private authService: AuthService, public location: Location) {
    this.currentUser = authService.getCurrentUser();
    this.currentUser.phoneNumber = this.getPhoneNumber();
  }

  ngOnInit() {
  }

  getPhoneNumber() {
    const length = this.currentUser.phoneNumber.length;
    return this.currentUser.phoneNumber.slice(4, length);
  }

  saveChanges() {
    this.currentUser.phoneNumber = '+358' + this.currentUser.phoneNumber;
    console.log(this.currentUser);
    this.authService.saveUserdata(this.currentUser)
      .subscribe(res => {
        console.log('user data changed');
        this.authService.setCurrentUserFromUserdata(this.currentUser);
        this.location.back();
      })
  }

  onCancel() {
    this.location.back();
  }

}
