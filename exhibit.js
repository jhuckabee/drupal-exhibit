// $Id: exhibit.js,v 1.2 2008/07/24 13:16:13 arto Exp $

/**
 * Enable auto facet generation based on fields in selected feeds in Exhibit node creation
 */
Drupal.behaviors.exhibit = function(context) {
  
  Drupal.exhibit.buildForm();
  // Loads all feeds, attaches change handler to feed checkboxes,
  // and initializes 
  Drupal.exhibit.loadFeeds(function(){
    $('input.exhibit-feed-checkbox:not(.exhibit-feed-checkbox-processed)', context).addClass('exhibit-feed-checkbox-processed').each(function () {
        $(this).bind('change', function(){
          Drupal.exhibit.selectFeed(this);
        });
        
        if (this.checked) {
          Drupal.exhibit.selectFeed(this);
        } 
    });
    
    // Bind insert button click event
    $('input#edit-insert-facet').bind('click', function(){
      Drupal.exhibit.insertFacet();
      return false;
    });
    
    $('#edit-exhibit-facet-generator-form-wrapper-field-name').bind('change', function(){
      Drupal.exhibit.filterFacetTypeByField();
      return false;
    });
    
    $('#edit-exhibit-facet-generator-form-wrapper-facet-type').bind('change', function(){
      Drupal.exhibit.facetTypeChange();
      return false;
    });
    
  });

};

/**
 * Facet form generator
 */
Drupal.exhibit = function() {
  
  var facets = {
                list:           {
                                  title:'List',
                                  exhibitClass: 'List',
                                  types: ['text','number','date','boolean','url','item'],
                                  options: {
                                            height:         {
                                                              title: 'Height',
                                                              help: "Height of the facet's body, e.g., \"20em\", \"200px\""
                                                            },
                                            sortMode:       {
                                                              title: 'Sort Mode',
                                                              help: "How to sort the choices in the facet",
                                                              choices: ['value', 'count']
                                                            },
                                            sortDirection:  {
                                                              title: 'Sort Direction',
                                                              help: "Whether to reverse the sort direction",
                                                              choices: ['forward', 'reverse']
                                                            },
                                            showMissing:    {
                                                              title: 'Show Missiong',
                                                              help: "Whether to provide a selection for items missing the facet -- this will suppress the '(missing this field)' text",
                                                              choices: ['true', 'false']
                                                            },
                                            selection:      {
                                                              title: 'Selection',
                                                              help: "Semicolon-separated list of default selections"
                                                            },
                                            fixedOrder:     {
                                                              title: 'Fixed Order',
                                                              help: "Semicolon-separated list of values specifying a fixed order for sorting the choices in the facet, e.g., \"Mo;Tu;We;Th;Fr\" for weekdays"
                                                            },
                                            scroll:         {
                                                              title: 'Scroll',
                                                              help: "If true, facet values are in a scrollable window of fixed size. If false, all facet values are shown in as much space as needed, without a scroll bar.",
                                                              choices: ['true', 'false']
                                                            },
                                            colorCoder:     {
                                                              title: 'Color Coder',
                                                              help: "Whcih color coder to use"
                                                            },
                                            collapsible:    {
                                                              title: 'Collapsible',
                                                              help: "Whether the facet is collapsible or not.",
                                                              choices: ['false', 'true'],
                                                              dependency: 'collapsed'
                                                            },
                                            collapsed:      {
                                                              title: 'Collapsed',
                                                              help: "Whether the facet is defaulted to a collapsed state.",
                                                              choices: ['false', 'true']
                                                            }
                                           }
                                },
                cloud:          {
                                 title: 'Cloud',
                                 exhibitClass: 'Cloud',
                                 types: ['text','number','date','boolean','url','item'],
                                 options: {
                                            selectMissing:    {
                                                              title: 'Select Missiong',
                                                              help: "Whether to provide a selection for items missing the facet -- this will suppress the '(missing this field)' text",
                                                              choices: ['true', 'false']
                                                            },
                                            selection:      {
                                                              title: 'Selection',
                                                              help: "Semicolon-separated list of default selections"
                                                            }
                                          }
                                },
                datePicker:     {
                                  title: 'Date Picker',
                                  exhibitClass: 'DatePicker',
                                  types: ['date'],
                                  options: {
                                             timerLimit:    {
                                                              title: 'Timer Limit',
                                                              help: "If set, this is the number of miliseconds that will pass between the first date selection and the auto selection of the second date. Timer limit will not apply if selection dragging is enabled."
                                                            },
                                             dragSelection: {
                                                              title: 'Enable Selection Dragging',
                                                              help: "If true, users will select a date range by clicking and dragging on the dates they want to pick.",
                                                              choices: ['false', 'true']
                                                            }
                                           }
                                 },
                hierarchical:   {
                                  title: 'Hierarchical',
                                  exhibitClass: 'Hierarchical',
                                  types: ['text','number','date','boolean','url','item'],
                                  options: {}
                                },
                numericRange:   {
                                  title: 'Numeric Range',
                                  exhibitClass: 'NumericRange',
                                  types: ['number'],
                                  options: {
                                            height:         {
                                                              title: 'Height',
                                                              help: "Height of the facet's body, e.g., \"20em\", \"200px\""
                                                            },
                                            collapsible:    {
                                                              title: 'Collapsible',
                                                              help: "Whether the facet is collapsible or not.",
                                                              choices: ['false', 'true'],
                                                              dependency: 'collapsed'
                                                            },
                                            collapsed:      {
                                                              title: 'Collapsed',
                                                              help: "Whether the facet is defaulted to a collapsed state.",
                                                              choices: ['false', 'true']
                                                            }
                                          }
                                 },
                slider:         {
                                  title: 'Slider',
                                  exhibitClass: 'Slider',
                                  types: ['number'],
                                  options: {
                                             scroll:         {
                                                               title: 'Scroll',
                                                               help: "If true, facet values are in a scrollable window of fixed size. If false, all facet values are shown in as much space as needed, without a scroll bar.",
                                                               choices: ['true', 'false']
                                                             },
                                             height:         {
                                                               title: 'Height',
                                                               help: "Height of the facet's body, e.g., \"20em\", \"200px\""
                                                             },
                                             precision:      {
                                                               title: 'Precision',
                                                               help: "Precision of the slider expressed as a whole or floating point number"
                                                             },
                                             historgram:     {
                                                               title: 'Histogram',
                                                               help: "Whether or not to show the histogram",
                                                               choices: ['true', 'false']
                                                             }
                                           }
                                },
                textSearch:     {
                                  title: 'Text Search',
                                  exhibitClass: 'TextSearch',
                                  types: ['text','number','date','boolean','url','item'],
                                  options: {
                                             queryParamName: {
                                                               title: 'Query Param Name',
                                                               help: "Which query param from the URL to auto filter by.  E.g. http://example.com?srch=something would automatically set the text search filter to 'something' and filter the results when the page is loaded."
                                                             },
                                             requiresEnter: {
                                                               title: 'Requires Enter',
                                                               help: "Whether the facet requires the user to hit the 'enter' key before filtering.",
                                                               choices: ['false', 'true']
                                                             }
                                           }
                                }
                };
  
  var feeds = {};
  
  var fieldWrapper = function(label, input, fieldId, helpText){
    return ['<div class="form-item">',
            '<label for="', fieldId, '">', label, '</label>',
            input,
            (helpText === undefined ? '' : '<div class="description">' + helpText + '</div>'),
            '</div>'].join('');
  };
  
  // Build list of facet type options
  var facetTypeOptions = function() {
    var options = [],
        selFieldName = $('#edit-exhibit-facet-generator-form-wrapper-field-name').val(),
        selField;
    
    // If a field name is selected, filter the facets by type 
    if ($('#edit-exhibit-facet-generator-form-wrapper-field-name') && selFieldName) {
      selField = getField(selFieldName);
      for (facet in facets) {
        if ($.inArray(selField.valueType, facets[facet].types) >= 0) {
         options.push('<option value="' + facet + '">' + facets[facet].title + '</option>');
        }
      }
    }
    else { // No field selected, return them all.  Used in initialization
      for (facet in facets) {
        options.push('<option value="' + facet + '">' + facets[facet].title + '</option>');
      }
    }
    return options.join('');
  };
  
  // Build array of unique fields based on selected feeds
  var getUniqueFields = function() {
    var fields = [];
    
    // Gather selected feed ids
    selectedIds = $.map($('input.exhibit-feed-checkbox:checked'), function(item) {
                    var parts = item.id.split('-');
                    return parts[parts.length-1];
    });
    
    
    // Build list of unique field names from the selected feeds
    $.each(selectedIds, function(selIdx, feed_id){
      $.each(feeds[feed_id].fields, function(feedsIdx, field){
        if($.inArray(field.name, fields) < 0) {
          fields.push(field.name);
        }
      });
    });
    
    return fields;
  };
  
  var getField = function(fieldName) {
    var selField;
    
    $.each(feeds, function(feedIdx, feed){
      $.each(feed.fields, function(fieldIdx, field){
       if (field.name === fieldName) {
         selField = field;
       }
      });
    });
    
    return selField;
  };
  
  return {
    
    // Build the default facet generation form
    buildForm: function() {
      $('div#facet-generator-form').html([fieldWrapper('Select Field: <span class="form-required" title="This field is required.">*</span>',
                                                       '<select name="exhibit[facet-generator-form-wrapper][field-name]" class="form-select required" id="edit-exhibit-facet-generator-form-wrapper-field-name" ></select>',
                                                       'edit-exhibit-facet-generator-form-wrapper-field-name'),
                                         fieldWrapper('Label: ',
                                                      '<input type="text" maxlength="128" name="exhibit[facet-generator-form-wrapper][facet-label]" id="edit-exhibit-facet-generator-form-wrapper-facet-label" size="60" value="" class="form-text" />',
                                                      'edit-exhibit-facet-generator-form-wrapper-facet-label'),
                                          fieldWrapper('Facet Type: <span class="form-required" title="This field is required.">*</span>',
                                                       ['<select name="exhibit[facet-generator-form-wrapper][facet-type]" class="form-select" id="edit-exhibit-facet-generator-form-wrapper-facet-type" >',
                                                       '<option></option>', facetTypeOptions() ,'</select>'].join(''),
                                                       'edit-exhibit-facet-generator-form-wrapper-facet-type'),
                                          '<div id="facet-options"></div>',
                                          '<input type="submit" name="insertFacet" id="edit-insert-facet" value="Generate Facet HTML" class="form-submit" />'].join('\n'));
    },
    
    // Build a facets options form
    buildFacetOptionsForm: function(facet) {
      var selFacet = facets[facet],
          formHtml = [];
          
      if (!facet || !selFacet) {
        return '';
      }
      
      for (option in selFacet.options) {
        // select field
        if (selFacet.options[option].choices &&
             selFacet.options[option].choices.length) {
          formHtml.push(fieldWrapper((selFacet.options[option].title ? selFacet.options[option].title : option) + ': ',
                                     ['<select name="exhibit[facet-generator-form-wrapper][', facet, '][', option, ']" ',
                                        'class="form-select" id="edit-exhibit-facet-generator-form-wrapper-', facet, '-', option, '" >',
                                        $.map(selFacet.options[option].choices, function(choice){
                                          return '<option value="' + choice + '">' + choice + '</option>';
                                        }).join(''),
                                        '</select>'].join(''),
                                      'edit-exhibit-facet-generator-form-wrapper-' + facet + '-' + option,
                                      selFacet.options[option].help));
        }
        else { // text field
          formHtml.push(fieldWrapper((selFacet.options[option].title ? selFacet.options[option].title : option) + ': ',
                                     '<input type="text" maxlength="128" name="exhibit[facet-generator-form-wrapper][' + facet + '][' + option + ']" id="edit-exhibit-facet-generator-form-wrapper-' + facet + '-' + option + '" size="60" value="" class="form-text" />',
                                     'edit-exhibit-facet-generator-form-wrapper-' + facet + '-' + option,
                                     selFacet.options[option].help));
        }
      }
      
      return formHtml.join('');

    },
    
    // Load all feeds
    loadFeeds: function(callback) {
      for (var feed_id in Drupal.settings.exhibit) {
        feeds[feed_id] = {loaded: false, url: Drupal.settings.exhibit[feed_id], fields: []};
      }

      for (feed_id in feeds){
        this.loadFeed(feeds[feed_id], callback);
      }
    },
    
    // Load an individual feed
    loadFeed: function(feed, callback) {
      var fieldType,
          that = this;
      
      $.ajax({
        type: "GET",
        url: '/' + feed.url,
        dataType: "json",
        success: function(data) {
          if (data.items && data.items[0]) {                                                // Make sure the feed contains items
            $.each(data.items, function(item){                                              // Loop through each item
              $.each(data.items[item], function(i) {                                        // Loop through each field in the item
                if ($.inArray(i, $.map(feed.fields, function(f){ return f.name;})) < 0) {   // Make sure the field doesn't already exist
                  if (data.properties && 
                      data.properties[i] && 
                      data.properties[i]['valueType']) {                                     // Check if item has a defined value type
                        fieldType = data.properties[i]['valueType'];                              // Use specified value type
                  }
                  else {
                    fieldType = 'item';                                                       // Use default 'item' value type
                  }
                  feed.fields.push({name: i, valueType: fieldType});                              // Add the field to the fields array
                }
              });
            });
          }
        },
        complete: function() {
          feed.loaded = true;

          if (typeof callback === 'function' &&
              that.allFeedsLoaded()) {
                callback();
          }
        }
      });
    
    },
    
    // Check if all feeds have been loaded
    allFeedsLoaded: function() {
      var loaded = true;
      $.each(feeds, function(feed_id){
        if (!feeds[feed_id].loaded) {
          loaded = false;
        }
      });
      return loaded;
    },
    
    // Handle feed selection
    selectFeed: function(element) {
      var selectedIds = [],
          options,
          optionsHtml = ['<option></option>'];
      
      // Hide facet generator form if no checkboxes are selected
      if (!$('input.exhibit-feed-checkbox:checked').size()){
        $('#exhibit-facet-builder').hide();
        return;
      }
      
      // Get fields
      options = getUniqueFields();
      
      // Show the facet generator form if we have fields
      if (options.length > 0){
        
        // Create the options list for each field and
        // update the field select box
        $.each(options, function(i, option){
          optionsHtml.push('<option value="' + option + '">' + option + '</option>');
        });
        $('#edit-exhibit-facet-generator-form-wrapper-field-name').html(optionsHtml.join(''));


        $('#exhibit-facet-builder').show();
      }
      else { // No fields for selected feeds, hide the form
        $('#exhibit-facet-builder').hide();
      }      
    },
    
    // Handles the facet type selection.
    // Displays the facet's options form
    facetTypeChange: function() {
      $('#facet-options').html(this.buildFacetOptionsForm($('#edit-exhibit-facet-generator-form-wrapper-facet-type').val()));
    },
    
    buildFacet: function() {
      if(!$('#edit-exhibit-facet-generator-form-wrapper-facet-type').val() || 
         !$('#edit-exhibit-facet-generator-form-wrapper-field-name').val()) {
        return '';
      }
      
      var facetType = $('#edit-exhibit-facet-generator-form-wrapper-facet-type').val(),
          facet = facets[facetType],
          facetOptionValue;
          
      if (!facet) {
        return '';
      }
      
      var facetHtml = ['<div class="facet" ex:role="facet"',
                       ' ex:expression=".', $('#edit-exhibit-facet-generator-form-wrapper-field-name').val(), '"'];
      
      if (facetType !== 'list'){
        facetHtml.push(' ex:facetClass="' + facet.exhibitClass + '"');
      }
       
      if ($('#edit-exhibit-facet-generator-form-wrapper-facet-label').val()){
       facetHtml.push(' ex:facetLabel="' + $('#edit-exhibit-facet-generator-form-wrapper-facet-label').val() + '"');
      }
      
      for (option in facet.options) {
        facetOptionValue = $('#edit-exhibit-facet-generator-form-wrapper-' + facetType + '-' + option).val();
        
        if (facetOptionValue) {
          if (facet.options[option].choices &&
              facet.options[option].choices.length && 
              facet.options[option].choices.length > 0) {
            if (facetOptionValue !== facet.options[option].choices[0]) {
              facetHtml.push(' ex:' + option + '="' + facetOptionValue + '"');
            }
          }
          else {
            facetHtml.push(' ex:' + option + '="' + facetOptionValue + '"');
          }
        }
      }
      
      facetHtml.push('></div>');
      return facetHtml.join('');
    },
    
    // Inserts the HTML for a facet based on the given options
    insertFacet: function() {
      var currentVal = '',
          val = [];
      currentVal = $('#edit-exhibit-facet-definition').val();
      if (currentVal != '') {
       val.push($.trim(currentVal)); 
      }
      val.push(this.buildFacet());
      $('#edit-exhibit-facet-definition').val(val.join('\n'));
    },
    
    filterFacetTypeByField: function() {
      $('#edit-exhibit-facet-generator-form-wrapper-facet-type').html('<option></option>' + facetTypeOptions());
      return false;
    }
    
  };
  
}();