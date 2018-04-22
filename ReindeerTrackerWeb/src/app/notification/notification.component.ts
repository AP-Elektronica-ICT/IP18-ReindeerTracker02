import { Component, OnInit } from '@angular/core';
import {Notification} from "../shared/classes/notification";
import {AuthService} from "../shared/auth.service";

@Component({
  selector: 'app-notification',
  templateUrl: './notification.component.html',
  styleUrls: ['./notification.component.css']
})
export class NotificationComponent implements OnInit {
  notifications: Notification[] = [];

  constructor(private authSerivce: AuthService) {
    this.notifications = authSerivce.getNotifications();
  }

  ngOnInit() {
  }

  ngOnDestroy() {
    if (this.authSerivce.isAuthenticated()) {
      this.authSerivce.markAllNotificationsAsSeen();
    }
  }

  getNotificationClass(seen: boolean) {
    if (!seen) {
      return 'list-group-item unseen';
    } else {
      return 'list-group-item';
    }
  }

  deleteNotification(index: number) {
    this.authSerivce.deleteNotification(index)
      .then((notifications: Notification[]) => {
        this.notifications = notifications;
      });
  }

  getRouterlink(url: string) {
    return url.split('?')[0];
  }

  getQueryParams(url: string) {
    console.log(url.split('?')[1], 'query');
    let fromUrl = url.split('?')[1];
    let name = fromUrl.split('=')[0];
    let value = fromUrl.split('=')[1];
    let jsonString = '{"'+name+'":'+value+'}';
    return JSON.parse(jsonString)
  }

}
