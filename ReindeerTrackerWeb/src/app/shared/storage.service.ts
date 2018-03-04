import {Inject, Injectable} from '@angular/core';
import * as firebase from 'firebase/app';
import 'firebase/storage'
import {reject} from "q";

@Injectable()
export class StorageService {
  temp: any;

  constructor() {

  }

  uploadImage(file: File, deviceKey: string): Promise<string> {
    return new Promise((resolve, reject) => {
      let storageRef = firebase.storage().ref('images');
      let uploadTask = storageRef.child(deviceKey).put(file);

      uploadTask.on(firebase.storage.TaskEvent.STATE_CHANGED,
        (snapshot) =>  {
          // upload in progress
        },
        (error) => {
          // upload failed
          console.log(error)
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
