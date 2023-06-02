boost::shared_mutex _access;
void reader() {
   // get shared access
   boost::shared_lock<boost::shared_mutex> lock(_access);
   // now we have shared access
}

void writer() {
   // get upgradable access
   boost::upgrade_lock<boost::shared_mutex> lock(_access);
   // get exclusive access
   boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
   // now we have exclusive access
}