module.exports = {
    "#get_food_type": function(intentVariables, sessionId, next) {
        var replyVariables = {};
        replyVariables['0'] = intentVariables['food_type0'];
        next(replyVariables);
    },
    "#confirm": function(intentVariables, sessionId, next) {
        var replyVariables = {};
        replyVariables['0'] = '5';
        next(replyVariables);
    }
}
