import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import {AngularFireAuth} from "angularfire2/auth";
import {Observable} from "rxjs/Observable";
import {Userdata} from "../../classes/userdata";
import {AppSettings} from "../../AppSettings";

/*
  Generated class for the AuthProvider provider.

  See https://angular.io/guide/dependency-injection for more info on providers
  and Angular DI.
*/
@Injectable()
export class AuthProvider {
  api = AppSettings.API_ENDPOINT;

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

  signupWithEmailPassword(email: string, password: string, data: Userdata): Observable<any> {
    const observable = new Observable(observer => {
      this.af.auth.createUserWithEmailAndPassword(email, password)
        .then(res => {
          data.uid = res.uid;
          console.log(res.uid);
          this.http.post(this.api + '/users', data)
            .subscribe(ress => {
              observer.next(ress);
              console.log(ress);
            }, err => {
              if (err.status >= 200 && err.status < 300) {
                observer.next();
              } else {
                observer.error(err);
              }
              console.log(err);
            })
        })
        .catch(err => {
          observer.error(err);
        })
    });
    return observable;
  }

  signOut(): Promise<any> {
    return this.af.auth.signOut();
  }
}
