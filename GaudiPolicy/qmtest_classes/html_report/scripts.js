// Global variables
var title = "QMTest Report";

// If set to false, the automatic polling of new data is interrupted.
var polling = true;

/**
 * Load the annotations file (annotations.json) and fill the annotations block of
 * the DOM.
 *
 * If the key "qmtest.run.end_time" is not present, the test is considered to be
 * running, so a new call to getData() is scheduled and the title of the page
 * is modified to show that it is running.
 *
 * Once the test is completed, the data are not polled anymore.
 */
function loadAnnotations() {
	// Asynchronous retrieval
	$.get('annotations.json', function(data) {
		// Prepare a table with the annotations
		var formatted = "<table>";
		// This keys are special and must appear first in the table
		var keys = ["qmtest.run.start_time", "qmtest.run.end_time"];
		// Add all the other keys to the list of keys
		for (var key in data) {
			if ($.inArray(key, keys) == -1) {
				keys.push(key);
			}
		}
		// Add the rows of the table
		var running = false;
		for (var i in keys) {
			var key = keys[i];
			var value = data[key];
			if (!value) { // In case of empty value for a key...
				if (key == "qmtest.run.end_time") {
					// ... if the key is "end_time", it means the tests are running
					value = "RUNNING".italics() +
					        " (the page will be updated every 5s)".small();
					running = polling;
				} else {
					// ... otherwise print a meaningful "None"
					value = "None".italics();
				}
			} else {
				if ($.isArray(value)) {
					// In case the value is an Array, let's format it as a list
					value = "<ul><li>" + value.join("</li><li>") + "</li></ul>";
				}
			}
			formatted += '<tr><td class="key">' + key + '</td><td class="value">'
						+ value + '</td></tr>';
		}
		formatted += "</table>";
		// Insert the code in the annotations block
		$('.annotations').html(formatted);

		if (running) {
			// modify the title
			$("title").html(title + " (running)");
			$("#title").html(title + " (running)");
			$("#stop_polling").show();
			// schedule an upload
			setTimeout(getData, 5000);
		} else {
			// set the title to the actual value
			$("title").html(title);
			$("#title").html(title);
			$("#stop_polling").hide();
		}
	}, "json");
}

/**
 * Update the summary block from a list of objects.
 *
 * The minimal requirement for the objects is that they have the field "outcome",
 * which value must be one of ["FAIL", "ERROR", "UNTESTED", "PASS"].
 *
 * @param summary
 */
function updateSummary(summary) {
	var counter = {"PASS":0,
			"FAIL":0,
			"UNTESTED":0,
			"ERROR":0};
	var total = summary.length;
	// Count the outcomes in the summary.
	for (var i in summary) {
		++counter[summary[i]["outcome"]];
	}
	// Prepare a table layout (like the one produced usually by QMTest).
	var html = "<table><tbody><tr><td align='right'>" + total
	           + "</td><td colspan='3'/><td>tests total</td></tr>";
	var result_types = ["FAIL", "ERROR", "UNTESTED", "PASS"];
	for (var i in result_types) {
		var result_type = result_types[i];
		if (counter[result_type]) {
			html += "<tr><td align='right'>" +
			        counter[result_type] +
			        "</td><td>(</td><td align='right'>" +
			        Math.round(counter[result_type] / total * 100) +
			        "%</td><td>)</td><td>tests " + result_type + "</td></tr>";
		}
	}
	html += "</tbody></table>";
	// Put the table in the summary block.
	$('.summary').html(html);
}

/**
 * Update the results from a list of objects.
 *
 * The format of the objects in the list must be:
 *
 * {"id": "...", "outcome": "...", "cause": "...", "fields": [...]}
 *
 * @param summary
 */
function updateResults(summary) {
	// Loop over each block with class "results" to fill it
	$(".results").each(function(){
		// Allow a custom regexp to select which outcomes to include.
		// The regexp must be included in the attribute "filter".
		var pattern = null;
		if ($(this).attr("filter"))
			pattern = new RegExp($(this).attr("filter"));
		// Top-level list
		var html = $("<ul/>");
		var any_match = false; // to store if we put anything in the list
		for (var i in summary) { // one item per test
			var test_data = summary[i];
			var outcome = test_data["outcome"];
			// operate only on the outcomes matching the pattern (if specified)
			if ( !pattern || outcome.match(pattern) ) {
				any_match = true;
				var it_res = $("<li/>").css("list-style-image", "url(plus.png)")
					.append($("<span/>").addClass("testid clickable")
						.append(test_data["id"]))
					.append(": ");
				var outcome_el = $("<span/>").addClass(outcome)
						.append(outcome);
				if (test_data["cause"]) {
					outcome_el.attr("title", test_data["cause"])
					.addClass("with_tooltip");
				}
				it_res.append(outcome_el);
				var fields_el = $("<div/>").addClass("fields").hide();
				if (test_data["cause"]) {
					fields_el.append($("<div/>").addClass("cause")
						.append(test_data["cause"]));
				}
				// Add the list of available fields for the test.
				var fields = summary[i]["fields"];
				var sublist = $("<ul/>");
				for (var j in fields) {
					sublist.append($("<li/>").css("list-style-image", "url(plus.png)")
						.append($("<span/>").addClass("fieldid clickable")
							.append(fields[j]))
						.append($("<div/>")
							.attr("href",test_data["id"] + "/" + fields[j])
							.attr("loaded", "false").hide()
							));
				}
				it_res.append(fields_el.append(sublist));
				html.append(it_res);
			}
		}
		if (any_match) {
			$(this).html(html)
			// add also a small link to collapse the tree
			.append($("<span>Collapse all</span>")
					.click(function(){
						$('.results div.fields').hide();
						$('.results div[href]').hide();
						$('.results span.testid').parent().css('list-style-image', 'url(plus.png)');
						$('.results span.fieldid').parent().css('list-style-image', 'url(plus.png)');
					}));
		} else {
			$(this).html("None.");
		}
	});
	// Instrument nodes
	$('.results span.testid').click(function(){
		var parent = $(this).parent();
		var child = parent.find('div.fields');
		if (child.is(':hidden')) {
			child.show();
			parent.css('list-style-image', 'url(minus.png)');
		} else {
			child.hide();
			parent.css('list-style-image', 'url(plus.png)');
		}
	});
	$('.results span.fieldid').click(function(){
		var parent = $(this).parent();
		var div = parent.find('div');
		if (div.attr("loaded") == "false") {
			div.load(div.attr("href"), function(){
				$(this).attr("loaded", "true");
			});
		}
		if (div.is(':hidden')) {
			div.show();
			parent.css('list-style-image', 'url(minus.png)');
		} else {
			div.hide();
			parent.css('list-style-image', 'url(plus.png)');
		}
	});
}
/**
 * Load the summary file "summary.json" and update the summary and result blocks.
 */
function loadSummary() {
	$.get("summary.json", function(summary) {
		updateSummary(summary);
		updateResults(summary);
	}, "json");
}

/**
 * Load all the data (both annotations and summary).
 */
function getData() {
	loadAnnotations();
	loadSummary();
}

// Function to be executed on load.
$('body').ready(function(){
	$(".hidable").hide();
	$(".hidable").before($("<span>(show)</span>").addClass("togglelink clickable"));
	$("span.togglelink").click(function(){
		var me = $(this);
		me.next().toggle();
		if (me.html() == "(show)") {
			me.html("(hide)");
		} else {
			me.html("(show)");
		}
	});
	$("#stop_polling").click(function(){
		polling = false;
		getData();
	});
	getData();
});
