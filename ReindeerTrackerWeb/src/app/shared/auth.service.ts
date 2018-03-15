import { Injectable } from '@angular/core';
import {AngularFireAuth} from "angularfire2/auth";

@Injectable()
export class AuthService {

  constructor(private af: AngularFireAuth) { }

  getCurrentUID(): string {
    //TODO get uid from firebase login
    return this.af.auth.currentUser.uid;
  }

  getAuthToken(): Promise<any> {
    return this.af.auth.currentUser.getIdToken();
  }

  loginWithEmailPassword(email: string, password: string): Promise<any> {
    return this.af.auth.signInWithEmailAndPassword(email, password)
  }

  signupWithEmailPassword(email: string, password: string): Promise<any> {
    return this.af.auth.createUserWithEmailAndPassword(email, password);
  }

  resetPassword(email: string): Promise<any> {
    return this.af.auth.sendPasswordResetEmail(email);
  }

}
