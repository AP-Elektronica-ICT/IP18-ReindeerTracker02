export interface Notification {
  _id: string,
  title: string,
  message: string,
  seen: boolean,
  link?: string,
  linkMessage?: string
}
