import { Injectable } from '@angular/core';
import * as firebase from 'firebase';

/*
  Generated class for the StorageProvider provider.

  See https://angular.io/guide/dependency-injection for more info on providers
  and Angular DI.
*/
@Injectable()
export class StorageProvider {

  temp: any;

  constructor() {

  }

  uploadImage(file: any, deviceKey: string): Promise<string> {
    return new Promise((resolve, reject) => {
      let storageRef = firebase.storage().ref('images');
      let uploadTask = storageRef.child(deviceKey).put(file);

      uploadTask.on(firebase.storage.TaskEvent.STATE_CHANGED,
        (snapshot) =>  {
          // upload in progress
        },
        (error) => {
          // upload failed
          reject(error);
          console.log(error);
        },
        () => {
          // upload success
          // console.log(uploadTask.metadata_.downloadURLs[0]);
          this.temp = uploadTask;
          resolve(this.temp.metadata_.downloadURLs[0]);
        }
      );
    })
  }

}
