(in-package :emotiq/config)

(defparameter *dns-ip-zt.emotiq.ch* 
  '((:hostname "zt-emq-01.zt.emotiq.ch"
     :ip "10.178.2.166"
     ;;; The ports take the default values if not otherwise specified
     :gossip-server-port 65002
     :rest-server-port 3140
     :websocket-server-port 3145)
    (:hostname "zt-emq-02.zt.emotiq.ch"
     :ip "10.178.0.71")
    (:hostname "zt-emq-03.zt.emotiq.ch"
     :ip "10.178.15.71")))

(defparameter *emotiq-conf*
  (make-pathname :defaults (emotiq/fs:etc/)
                 :name "emotiq-conf"
                 :type "json"))

(um:defconstant+ +default-configuration+
    `((:rest-server
       . :true)
      (:rest-server-port
       . 3140)
      (:websocket-server
       . :true)
      (:websocket-server-port
       . 3145)
      (:gossip-server
       . :true)
      (:gossip-server-port
       . 65002)
      (:genesis-block
       . "emotiq-genesis-block.json")))

(defun generated-directory (configuration)
  (let ((host (alexandria:assoc-value configuration :hostname))
        (ip (alexandria:assoc-value configuration :ip))
        (gossip-server-port (alexandria:assoc-value configuration :gossip-server-port))
        (rest-server-port (alexandria:assoc-value configuration :rest-server-port))
        (websocket-server-port (alexandria:assoc-value configuration :websocket-server-port)))
    (make-pathname
     :directory `(:relative
                  ,(format nil "~{~a~^-~}"
                           (list host ip gossip-server-port rest-server-port websocket-server-port)))
     :defaults nil)))

(defun network/generate (&key
                           (root (emotiq/fs:tmp/))
                           (nodes-dns-ip *dns-ip-zt.emotiq.ch*)
                           (force nil)
                           (settings-key-value-alist nil))
  "Generate a test network configuration"
  (let* ((nodes (keys/generate nodes-dns-ip))
         (stakes (stakes/generate nodes))
         directories)
    (dolist (node nodes)
      (let ((configuration
             (copy-alist +default-configuration+))
            (hostname
             (getf node :hostname))
            (ip
             (getf node :ip)))
        (push (cons :hostname hostname)
              configuration)
        (push (cons :ip ip)
              configuration)
        (push `(:public
                . ,(random 100)) ;; FIXME
              configuration)
        (push `(:private
                . ,(random 100)) ;; FIXME
              configuration)
        #+(or) ;; FIXME
        (push `(:witnesses-and-stakes
                . ,stakes)
              configuration)
        ;;; Override by pushing ahead in alist
        (when settings-key-value-alist
          (loop :for (key . value)
             :in settings-key-value-alist
             :doing (push `(,key . ,value) configuration)))

        (let ((relative-path (generated-directory configuration)))
          (let ((path (merge-pathnames relative-path root))
                (configuration (copy-alist configuration)))
            (push 
             (node/generate path
                            configuration
                            :force force
                            :key-records nodes)
             directories)))))
    directories))

(defun node/generate (directory
                      configuration
                      &key
                        witnesses-and-stakes
                        key-records
                        (force nil))
  "Generate a compete Emotiq node description within DIRECTORY for CONFIGURATION"
  (gossip/config:generate-node
   :root directory
   :host (alexandria:assoc-value configuration :host)
   :eripa (alexandria:assoc-value configuration :ip)
   :gossip-port (alexandria:assoc-value configuration :gossip-server-port)
   :public (alexandria:assoc-value configuration :public)
   :private (alexandria:assoc-value configuration :private)
   :key-records key-records)
  (with-open-file (o (make-pathname :defaults directory
                                    :name "emotiq-config"
                                    :type "json")
                     :if-exists :supersede
                     :direction :output)
    (cl-json:encode-json configuration o))
  (stakes/write key-records
                :path (make-pathname :defaults directory
                                     :name "stakes"
                                     :type "conf"))
  (genesis/create configuration :root directory))
  
(defun settings/read () 
  (unless (probe-file *emotiq-conf*)
    (emotiq:note "No configuration able to be read from '~a'" *emotiq-conf*)
    (return-from settings/read nil))
  ;;; TODO: do gossip/configuration sequence
  (cl-json:decode-json-from-source *emotiq-conf*))


