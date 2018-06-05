#ifndef GL_DRAW2D_H
#define GL_DRAW2D_H


#include "opengl.hpp"
#include "shader.hpp"

namespace Color {
    static const glm::vec4 RED = {1.0, 0.0, 0.0, 1.0};
    static const glm::vec4 GREEN = {0.0, 1.0, 0.0, 1.0};
    static const glm::vec4 BLUE = {0.0, 0.0, 1.0, 1.0};
    static const glm::vec4 MAGENTA = {1.0, 0.0, 1.0, 1.0};
    static const glm::vec4 BLACK = {0.0, 0.0, 0.0, 1.0};    
}

struct GLDraw2D {

    static constexpr GLuint ATTRIBUTE_STRIDE = 7;

    GLfloat vertices_triangles[GL_DRAW2D_SIZE * ATTRIBUTE_STRIDE]; 
    GLuint indices_triangles[GL_DRAW2D_SIZE * 2];

    GLfloat vertices_lines[GL_DRAW2D_SIZE * ATTRIBUTE_STRIDE]; 
    GLuint indices_lines[GL_DRAW2D_SIZE * 2];

    VertexAttributeArray vao_triangles;
    VertexBufferData triangle_buffer;
    VertexAttributeArray vao_lines;
    VertexBufferData line_buffer;
    Shader shader;

    UniformLocation MAT_LOC;

    glm::mat4 projection_matrix;
    glm::mat4 transform_matrix;

    glm::vec4 color;

    bool update_projection_matrix;

    GLenum draw_type;


    GLuint index_triangles;
    GLuint index_lines;

    bool begun;

    void begin(void) 
    {
        assert(begun == false);
        
        transform_matrix = glm::mat4(1.0f);

        glUseProgram(shader);

        if (update_projection_matrix) {
            update_projection_matrix = false;
            glUniformMatrix4fv(MAT_LOC, 1, GL_FALSE, glm::value_ptr(projection_matrix));
        }

        begun = true;
    }

    void end(void) 
    {
        assert(begun == true);

        glBindVertexArray(vao_triangles);
        gl_bind_buffers_and_upload_sub_data(&triangle_buffer);
        if (triangle_buffer.i_count > 0) { 
            glDrawElements(GL_TRIANGLES, triangle_buffer.i_count, GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(vao_lines);
        gl_bind_buffers_and_upload_sub_data(&line_buffer);
        if (line_buffer.i_count > 0) { 
            glDrawElements(GL_LINES, line_buffer.i_count, GL_UNSIGNED_INT, 0);
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);


        triangle_buffer.v_count = 0;
        triangle_buffer.i_count = 0;

        line_buffer.v_count = 0;
        line_buffer.i_count = 0;

        index_triangles = 0;
        index_lines = 0;

        begun = false;
    }

    void end_no_reset(void) 
    {
        assert(begun == true);

        glBindVertexArray(vao_triangles);
        gl_bind_buffers_and_upload_sub_data(&triangle_buffer);
        if (triangle_buffer.i_count > 0) { 
            glDrawElements(GL_TRIANGLES, triangle_buffer.i_count, GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(vao_lines);
        gl_bind_buffers_and_upload_sub_data(&line_buffer);
        if (line_buffer.i_count > 0) { 
            glDrawElements(GL_LINES, line_buffer.i_count, GL_UNSIGNED_INT, 0);
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);

        begun = false;        
    }

    static constexpr const char* const SHADER_VERTEX_PATH = "shaders/default_2d/default_2d.vrts";
    static constexpr const char* const SHADER_FRAGMENT_PATH = "shaders/default_2d/default_2d.frgs";

    bool init(GLDraw2D* ctx, glm::mat4 projection_matrix)
    {

    }
    bool init(glm::mat4 projection_matrix)
    {
        GLDraw2D::projection_matrix = projection_matrix;
        update_projection_matrix = false;
        begun = false;

        index_triangles = 0;
        index_lines = 0;

        draw_type = GL_TRIANGLES;

        color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        shader.load_from_file(
            SHADER_VERTEX_PATH,
            SHADER_FRAGMENT_PATH
        );

        if (!shader.is_valid()) {
            fprintf(stderr, "ERROR: immediate mode failed\n");
            return false;
        }

        glUseProgram(shader);
        MAT_LOC = glGetUniformLocation(shader, "u_matrix");
        glUniformMatrix4fv(MAT_LOC, 1, GL_FALSE, glm::value_ptr(projection_matrix));
        glUseProgram(0);

        VertexAttributeArray_init(&vao_triangles, ATTRIBUTE_STRIDE);
        glBindVertexArray(vao_triangles);

            VertexBufferData_init_inplace(
                &triangle_buffer, 
                GL_DRAW2D_SIZE * ATTRIBUTE_STRIDE,
                vertices_triangles,
                GL_DRAW2D_SIZE,
                indices_triangles
            );
            triangle_buffer.v_count = 0;
            triangle_buffer.i_count = 0;

            gl_bind_buffers_and_upload_data(&triangle_buffer, GL_STREAM_DRAW);
            // POSITION
            gl_set_and_enable_vertex_attrib_ptr(0, 3, GL_FLOAT, GL_FALSE, 0, &vao_triangles);
            // COLOR
            gl_set_and_enable_vertex_attrib_ptr(1, 4, GL_FLOAT, GL_FALSE, 3, &vao_triangles);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        VertexAttributeArray_init(&vao_lines, ATTRIBUTE_STRIDE);
        glBindVertexArray(vao_lines);
            VertexBufferData_init_inplace(
                &line_buffer, 
                GL_DRAW2D_SIZE * ATTRIBUTE_STRIDE,
                vertices_lines,
                GL_DRAW2D_SIZE,
                indices_lines
            );
            line_buffer.v_count = 0;
            line_buffer.i_count = 0;

            gl_bind_buffers_and_upload_data(&line_buffer, GL_STREAM_DRAW);
            // POSITION
            gl_set_and_enable_vertex_attrib_ptr(0, 3, GL_FLOAT, GL_FALSE, 0, &vao_lines);
            // COLOR
            gl_set_and_enable_vertex_attrib_ptr(1, 4, GL_FLOAT, GL_FALSE, 3, &vao_lines);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        index_triangles = 0;
        index_lines = 0;

        return true;
    }

    void free(void)
    {
        VertexAttributeArray_delete(&vao_triangles);
        VertexAttributeArray_delete(&vao_lines);
        VertexBufferData_delete_inplace(&triangle_buffer);
        VertexBufferData_delete_inplace(&line_buffer);
    }

    void line(glm::vec3 a, glm::vec3 b)
    {
        const size_t v_count = line_buffer.v_count;
        const size_t i_count = line_buffer.i_count;

        if (v_count + (2 * ATTRIBUTE_STRIDE) > GL_DRAW2D_SIZE * ATTRIBUTE_STRIDE || i_count + 2 > GL_DRAW2D_SIZE * 2) {
            fprintf(stderr, "%s\n", "ERROR: add_line_segment MAX LINES EXCEEDED");
            return;
        }

        a = glm::vec3(transform_matrix * glm::vec4(a, 1.0f));
        b = glm::vec3(transform_matrix * glm::vec4(b, 1.0f));

        const size_t v_idx = v_count;

        memcpy(&vertices_lines[v_idx], &a[0], sizeof(a[0]) * 3);
        memcpy(&vertices_lines[v_idx + 3], &color[0], sizeof(color[0]) * 4);


        memcpy(&vertices_lines[v_idx + ATTRIBUTE_STRIDE], &b[0], sizeof(b[0]) * 3);
        memcpy(&vertices_lines[v_idx + ATTRIBUTE_STRIDE + 3], &color[0], sizeof(color[0]) * 4);

        indices_lines[i_count] = index_lines;
        indices_lines[i_count + 1] = index_lines + 1;
        index_lines += 2;

        line_buffer.v_count += (2 * ATTRIBUTE_STRIDE);
        line_buffer.i_count += 2;

        // std::cout << "BEGIN" << std::endl;
        // std::cout << "{" << std::endl;
        // for (size_t i = 0; i < line_buffer.v_count; ++i) {
        //     std::cout << line_buffer.vertices[i] << ", ";
        // }
        // std::cout << std::endl << "}" << std::endl;

        // std::cout << "{" << std::endl;
        // for (size_t i = 0; i < line_buffer.i_count; ++i) {
        //     std::cout << line_buffer.indices[i] << ", ";
        // }
        // std::cout << std::endl << "}" << std::endl;
        // std::cout << "END" << std::endl;
    }

    void line(glm::vec2 a, glm::vec2 b)
    {
        const size_t v_count = line_buffer.v_count;
        const size_t i_count = line_buffer.i_count;

        if (v_count + (2 * ATTRIBUTE_STRIDE) > GL_DRAW2D_SIZE * ATTRIBUTE_STRIDE || i_count + 2 > GL_DRAW2D_SIZE * 2) {
            fprintf(stderr, "%s\n", "ERROR: add_line_segment MAX LINES EXCEEDED");
            return;
        }

        a = glm::vec2(transform_matrix * glm::vec4(a, 0.0f, 1.0f));
        b = glm::vec2(transform_matrix * glm::vec4(b, 0.0f, 1.0f));

        const size_t v_idx = v_count;

        memcpy(&vertices_lines[v_idx], &a[0], sizeof(a[0]) * 2);
        vertices_lines[v_idx + 2] = 0.0f;
        memcpy(&vertices_lines[v_idx + 3], &color[0], sizeof(color[0]) * 4);


        memcpy(&vertices_lines[v_idx + ATTRIBUTE_STRIDE], &b[0], sizeof(b[0]) * 2);
        vertices_lines[v_idx + ATTRIBUTE_STRIDE + 2] = 0.0f;
        memcpy(&vertices_lines[v_idx + ATTRIBUTE_STRIDE + 3], &color[0], sizeof(color[0]) * 4);

        indices_lines[i_count] = index_lines;
        indices_lines[i_count + 1] = index_lines + 1;
        index_lines += 2;

        line_buffer.v_count += (2 * ATTRIBUTE_STRIDE);
        line_buffer.i_count += 2;
    }

    void polygon_convex_regular(GLfloat radius, glm::vec3 center, const size_t count_sides)
    {
        size_t count_tris = count_sides - 2;

        const size_t inc = ATTRIBUTE_STRIDE;

        size_t v_count = 0;
        size_t i_count = 0;
        size_t v_idx = 0;
        GLdouble angle_turn = angle_turn = (2 * glm::pi<GLdouble>()) / count_sides;

        switch (draw_type) {
        case GL_TRIANGLES:
            v_count = triangle_buffer.v_count;
            i_count = triangle_buffer.i_count;
            v_idx = v_count;

            if (v_count + (ATTRIBUTE_STRIDE * count_sides) > GL_DRAW2D_SIZE * ATTRIBUTE_STRIDE || i_count + (3 * count_tris) > GL_DRAW2D_SIZE * 2) {
                fprintf(stderr, "%s\n", "ERROR: polygon_convex_regular MAX TRIANGLES EXCEEDED");
                return;
            }

            for (size_t p = 0, idx_off = 0; p < count_tris; ++p, idx_off += 3) {
                indices_triangles[i_count + idx_off]     = index_triangles + 0;
                indices_triangles[i_count + idx_off + 1] = index_triangles + p + 1;
                indices_triangles[i_count + idx_off + 2] = index_triangles + p + 2;
            }
            triangle_buffer.i_count += (3 * count_tris);


            for (size_t p = 0, off = 0; p < count_sides; ++p, off += inc) {
                glm::vec3 point = glm::vec3(transform_matrix * 
                    glm::vec4(
                        (radius * glm::cos(p * angle_turn)) + center.x,
                        (radius * glm::sin(p * angle_turn)) + center.y,
                        center.z,
                        1.0f
                    )
                );
                vertices_triangles[v_idx + off]     = point.x;
                vertices_triangles[v_idx + off + 1] = point.y;
                vertices_triangles[v_idx + off + 2] = point.z;

                memcpy(&vertices_triangles[v_idx + off + 3], &color[0], sizeof(color[0]) * 4);
            }

            triangle_buffer.v_count += (ATTRIBUTE_STRIDE * count_sides);

            index_triangles += count_sides;

            break;
        case GL_LINES:
            v_count = line_buffer.v_count;
            i_count = line_buffer.i_count;
            v_idx = v_count;

            if (v_count + (ATTRIBUTE_STRIDE * count_sides) > GL_DRAW2D_SIZE * ATTRIBUTE_STRIDE || i_count + (2 * count_sides) > GL_DRAW2D_SIZE * 2) {
                fprintf(stderr, "%s\n", "ERROR: polygon_convex_regular MAX LINES EXCEEDED");
                return;
            }

            for (size_t p = 0, off = 0; p < count_sides; ++p, off += 2) {
                indices_lines[i_count + off]     = index_lines + p;
                indices_lines[i_count + off + 1] = index_lines + p + 1;
            }
            indices_lines[i_count + (count_sides * 2) - 1] = index_lines;

            line_buffer.i_count += (2 * count_sides);

            for (size_t p = 0, off = 0; p < count_sides; ++p, off += inc) {
                glm::vec3 point = glm::vec3(transform_matrix * 
                    glm::vec4(
                        (radius * glm::cos(p * angle_turn)) + center.x,
                        (radius * glm::sin(p * angle_turn)) + center.y,
                        center.z,
                        1.0f
                    )
                );
                vertices_lines[v_idx + off]     = point.x;
                vertices_lines[v_idx + off + 1] = point.y;
                vertices_lines[v_idx + off + 2] = point.z;

                memcpy(&vertices_lines[v_idx + off + 3], &color[0], sizeof(color[0]) * 4);
            }

            line_buffer.v_count += (ATTRIBUTE_STRIDE * count_sides);          

            index_lines += count_sides; 

            break;
        }
    }

    void circle(GLfloat radius, glm::vec3 center)
    {
        polygon_convex_regular(radius, center, 37);
    }

    void vertex(glm::vec3 v)
    {
        size_t v_count = 0;
        size_t i_count = 0;
        size_t v_idx = 0;

        switch (draw_type) {
        case GL_TRIANGLES:
            v_count = triangle_buffer.v_count;
            i_count = triangle_buffer.i_count;
            v_idx = v_count;

            if (v_count + ATTRIBUTE_STRIDE > GL_DRAW2D_SIZE * ATTRIBUTE_STRIDE || i_count + 1 > GL_DRAW2D_SIZE * 2) {
                fprintf(stderr, "%s\n", "ERROR: vertex MAX TRIANGLES EXCEEDED");
                return;
            }

            v = glm::vec3(transform_matrix * glm::vec4(v, 1.0f));


            memcpy(&vertices_triangles[v_idx], &v[0], sizeof(v[0]) * 3);
            memcpy(&vertices_triangles[v_idx + 3], &color[0], sizeof(color[0]) * 4);

            indices_triangles[i_count] = index_triangles;
            ++index_triangles;

            triangle_buffer.v_count += ATTRIBUTE_STRIDE;
            ++triangle_buffer.i_count;

            // std::cout << "BEGIN" << std::endl;
            // std::cout << "{" << std::endl;
            // for (size_t i = 0; i < triangle_buffer.v_count; ++i) {
            //     std::cout << triangle_buffer.vertices[i] << ", ";
            // }
            // std::cout << std::endl << "}" << std::endl;

            // std::cout << "{" << std::endl;
            // for (size_t i = 0; i < triangle_buffer.i_count; ++i) {
            //     std::cout << triangle_buffer.indices[i] << ", ";
            // }
            // std::cout << std::endl << "}" << std::endl;
            // std::cout << "END" << std::endl;

            break;
        case GL_LINES:
            v_count = line_buffer.v_count;
            i_count = line_buffer.i_count;
            v_idx = v_count;

            if (v_count + ATTRIBUTE_STRIDE > GL_DRAW2D_SIZE * ATTRIBUTE_STRIDE || i_count + 1 > GL_DRAW2D_SIZE * 2) {
                fprintf(stderr, "%s\n", "ERROR: vertex MAX LINES EXCEEDED");
                return;
            }

            v = glm::vec3(transform_matrix * glm::vec4(v, 1.0f));


            memcpy(&vertices_lines[v_idx], &v[0], sizeof(v[0]) * 3);
            memcpy(&vertices_lines[v_idx + 3], &color[0], sizeof(color[0]) * 4);

            indices_lines[i_count] = index_lines;
            ++index_lines;

            line_buffer.v_count += ATTRIBUTE_STRIDE;
            ++line_buffer.i_count;

            break;
        }      
    }
};

#endif // GL_DRAW_H
