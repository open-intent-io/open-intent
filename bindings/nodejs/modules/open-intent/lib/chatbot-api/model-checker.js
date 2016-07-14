module.exports = checkModel;

function checkModel(model) {
    if(!model.dictionary()) {
        return Error('Missing dictionary.');
    }

    if(typeof model.dictionary() !== 'string') {
        return Error('Dictionary must be a string.');
    }

    if(! model.oiml()) {
        return Error('Missing OIML script.');
    }

    if(typeof model.oiml() !== 'string') {
        return Error('OIML script must be a string.');
    }

    return undefined;
}