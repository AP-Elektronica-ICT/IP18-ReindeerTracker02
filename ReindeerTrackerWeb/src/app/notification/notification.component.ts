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
    console.log('destroy');
    this.authSerivce.markAllNotificationsAsSeen();
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

}
