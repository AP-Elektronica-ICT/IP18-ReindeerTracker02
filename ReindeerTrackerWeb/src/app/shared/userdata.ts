import {Notification} from "./classes/notification";

export interface Userdata {
  email: string,
  firstName: string,
  lastName: string,
  birthdate: Date,
  phoneNumber: string,
  location: string,
  uid?: string,
  deviceToken?: string,
  admin?: boolean,
  notifications?: Notification[],
  unseen?: number
}
