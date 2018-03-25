import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import {Camera, CameraOptions} from "@ionic-native/camera";

/*
  Generated class for the ImageProvider provider.

  See https://angular.io/guide/dependency-injection for more info on providers
  and Angular DI.
*/
@Injectable()
export class ImageProvider {
  public cameraImage : String

  constructor(public http: HttpClient, private _CAMERA : Camera) {
    console.log('Hello ImageProvider Provider');
  }

  selectImage() : Promise<any>
  {
    return new Promise(resolve =>
    {
      let cameraOptions : CameraOptions = {
        sourceType         : this._CAMERA.PictureSourceType.PHOTOLIBRARY,
        destinationType    : this._CAMERA.DestinationType.DATA_URL,
        quality            : 100
      };

      this._CAMERA.getPicture(cameraOptions)
        .then((data) =>
        {
          this.cameraImage 	= "data:image/jpeg;base64," + data;
          resolve(this.cameraImage);
        });


    });
  }
}
