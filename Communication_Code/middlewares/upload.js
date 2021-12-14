const multer = require('multer');

const storage = multer.diskStorage({
    destination: function (req, file, cb) {
        cb(null, 'public/');
    },
    filename: function (req, file, cb) {
        cb(null, file.originalname);
    },
    onError: function (err, next) {
        next(err);
    }
});


const upload = multer({ storage }).any('file');

module.exports = upload;