var food_type;

module.exports = {
    "#get_food_type": function(intentVariables, sessionId, next) {
        var replyVariables = {};
        food_type = intentVariables['food_type0'];
        replyVariables['0'] = food_type;
        next(replyVariables);
    },
    "#confirm": function(intentVariables, sessionId, next) {
        var replyVariables = {};
        if(food_type == 'pizza') {
            replyVariables['0'] = '8';
        }
        else {
            replyVariables['0'] = '5';
        }
        next(replyVariables);
    }
}
