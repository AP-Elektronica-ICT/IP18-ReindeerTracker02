import { Component } from '@angular/core';
import * as firebase from 'firebase/app';
import {environment} from "../environments/environment";
import {AngularFireAuth} from "angularfire2/auth";
import {AuthService} from "./shared/auth.service";

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent {
  loaded = false;

  constructor(private af: AngularFireAuth, private authh: AuthService) {
    // firebase.initializeApp(environment.firebase);
    af.auth.onAuthStateChanged((auth) => {
      console.log(auth);
      if (auth) {
        this.authh.setCurrentUser()
          .then(() => {
            this.loaded = true;
            this.authh.startNotificationRefresh();
          });
      } else {
        this.loaded = true;
      }
    })
  }
}
