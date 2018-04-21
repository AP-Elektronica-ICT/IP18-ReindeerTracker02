import { Component } from '@angular/core';
import {ActionSheetController, IonicPage, NavController, NavParams} from 'ionic-angular';
import {NotificationProvider} from "../../providers/notification/notification";
import {Notification} from "../../classes/notification";

/**
 * Generated class for the NotificationPage page.
 *
 * See https://ionicframework.com/docs/components/#navigation for more info on
 * Ionic pages and navigation.
 */

@IonicPage()
@Component({
  selector: 'page-notification',
  templateUrl: 'notification.html',
})
export class NotificationPage {
  notifications: Notification[] = [];

  constructor(public navCtrl: NavController, public navParams: NavParams, private notificationProvider: NotificationProvider, private actionSheetCrtl: ActionSheetController) {
  }

  ionViewDidLoad() {
    this.getNotifications();
  }

  getNotifications() {
    this.notificationProvider.getNotifications()
      .subscribe((notifications: Notification[]) => {
        this.notifications = notifications;
        console.log(this.notifications);
      })
  }

  openDelete(notificationID: string, index: number) {
    let actionSheet = this.actionSheetCrtl.create({
      title: 'Notification',
      buttons: [
        {
          text: 'Delete',
          icon: 'trash',
          handler: () => {
            this.notificationProvider.deleteNotification(notificationID)
              .subscribe(res => {
                this.notifications.splice(index, 1);
              })
          }
        },{
          text: 'Mark as seen',
          icon: 'eye',
          handler: () => {
            this.notificationProvider.markNotificationsAsSeen([notificationID])
              .subscribe(res => {
                this.notifications[index].seen = true;
              })
          }
        },{
          text: 'Cancel',
          icon: 'close',
          role: 'cancel',
          handler: () => {
            console.log('Cancel clicked');
          }
        }
      ]
    });
    actionSheet.present();
  }

}
