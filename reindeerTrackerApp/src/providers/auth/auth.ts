import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import {AngularFireAuth} from "angularfire2/auth";

/*
  Generated class for the AuthProvider provider.

  See https://angular.io/guide/dependency-injection for more info on providers
  and Angular DI.
*/
@Injectable()
export class AuthProvider {

  constructor(public http: HttpClient, public af: AngularFireAuth) {

  }

  getCurrentUID(): string {
    return this.af.auth.currentUser.uid;
  }

  isAuthenticated(): boolean {
    if (this.af.auth.currentUser) {
      return true;
    } else {
      return false;
    }
  }

  loginWithEmailPassword(email: string, password: string): Promise<any> {
    return this.af.auth.signInWithEmailAndPassword(email, password)
  }

  signOut(): Promise<any> {
    return this.af.auth.signOut();
  }
}
